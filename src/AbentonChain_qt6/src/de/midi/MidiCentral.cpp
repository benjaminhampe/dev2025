/*
 *  MidiCentral.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include <de/midi/MidiCentral.h>
#include <cstdint>
#include <algorithm>
#include <sstream>
#include <thread>

#include <RtMidi/RtMidi.h>

namespace de {
namespace midi {

// =======================================================
struct MidiUtil
// =======================================================
{
    static RtMidiIn* openMidiIn( int portIndex )
    {
        RtMidiIn* p = nullptr;
        int portCount = 0;
        try
        {
            p = new RtMidiIn( RtMidi::WINDOWS_MM, "RtMidiInputEnum" );
            portCount = p->getPortCount();
            if (portCount < 1 || portIndex >= portCount)
            {
                DE_OK("Invalid midi input port ",portIndex," of ",portCount)
                delete p;
                return nullptr;
            }
            p->openPort( portIndex );
            DE_OK("Opened midi input port ",portIndex," of ",portCount)
        }
        catch ( RtMidiError & e )
        {
            DE_ERROR("RtMidiException: ", e.getMessage().c_str() )
            if ( p )
            {
                delete p;
                p = nullptr;
            }
        }

        return p;
    }
    static RtMidiOut* openMidiOut( int portIndex )
    {
        RtMidiOut* p = nullptr;
        int portCount = 0;
        try
        {
            p = new RtMidiOut( RtMidi::WINDOWS_MM, "RtMidiOutputEnum" );
            portCount = p->getPortCount();
            if (portCount < 1 || portIndex >= portCount)
            {
                DE_OK("Invalid midi output port ",portIndex," of ",portCount)
                delete p;
                return nullptr;
            }
            p->openPort( portIndex );
            DE_OK("Opened midi output port ",portIndex," of ",portCount)
        }
        catch ( RtMidiError & e )
        {
            DE_ERROR("RtMidiException: ", e.getMessage().c_str() )
            if ( p )
            {
                delete p;
                p = nullptr;
            }
        }

        return p;
    }

    static std::vector< std::string >
    enumerateMidiOut()
    {
        std::vector< std::string > names;
        RtMidiOut* p = nullptr;
        try
        {
            p = new RtMidiOut( RtMidi::WINDOWS_MM, "RtMidiOutputEnum" );
            const uint32_t nPorts = p->getPortCount();
            if ( nPorts )
            {
                names.reserve( nPorts );
                for ( uint32_t i = 0; i < nPorts; i++ )
                {
                    names.emplace_back( p->getPortName( i ) );
                }
            }
            if ( p ) { delete p; }
        }
        catch ( RtMidiError & e )
        {
            DE_ERROR("RtMidiException: ", e.getMessage() )
            if ( p ) { delete p; }
        }
        return names;
    }

    static std::vector< std::string >
    enumerateMidiIn()
    {
        std::vector< std::string > names;
        RtMidiIn* p = nullptr;
        try
        {
            p = new RtMidiIn( RtMidi::WINDOWS_MM, "RtMidiInputEnum" );
            const uint32_t nPorts = p->getPortCount();
            if ( nPorts )
            {
                names.reserve( nPorts );
                for ( uint32_t i = 0; i < nPorts; i++ )
                {
                    names.emplace_back( p->getPortName( i ) );
                }
            }
            if ( p ) { delete p; }
        }
        catch ( RtMidiError & e )
        {
            DE_ERROR("RtMidiException: ", e.getMessage() )
            if ( p ) { delete p; }
        }
        return names;
    }

    static void printMidiOut()
    {
        const std::vector< std::string > v = enumerateMidiOut();
        DE_TRACE("PortCount = ", int(v.size()) )
        for ( size_t i = 0; i < v.size(); ++i )
        {
            DE_DEBUG("Port[",int(i),"] ", v[i])
        }
    }

    static void printMidiIn()
    {
        const std::vector< std::string > v = enumerateMidiIn();
        DE_TRACE("PortCount = ", int(v.size()) )
        for ( size_t i = 0; i < v.size(); ++i )
        {
            DE_DEBUG("Port[",int(i),"] ", v[i])
        }
    }
};

// =======================================================
struct MidiCentral_Private
// =======================================================
{
    RtMidiIn*   m_midiIn;
    RtMidiOut*  m_midiOut;
    uint32_t 	m_portIn;
    uint32_t 	m_portOut;

    std::vector< uint8_t > m_midiInBuffer;
    std::vector< uint8_t > m_midiOutBuffer;

    std::vector<IMidiMessageListener*> m_listeners;
    //std::vector<ShortMidiMessageListener> m_shortListeners;
    //std::vector<MidiMessageListener> m_longListeners;
    std::mutex m_mutex;

    MidiCentral_Private()
        : m_midiIn(nullptr)
        , m_midiOut(nullptr)
        , m_portIn(0)
        , m_portOut(0)
    {
        m_midiInBuffer.reserve( 128 );
        m_midiOutBuffer.reserve( 128 );
        m_listeners.reserve( 128 );
        setup();
    }

    ~MidiCentral_Private()
    {
        close();
    }

    void setup()
    {
        MidiUtil::printMidiIn();
        MidiUtil::printMidiOut();

        openInput( 0 );
        openOutput( 0 );
    }

    void close()
    {
        closeOutput();
    }

    void registerListener(IMidiMessageListener* l)
    {
        if (!l)
        {
            DE_ERROR("Got nullptr")
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_listeners.push_back(l);
    }

    void deregisterListener(IMidiMessageListener* l)
    {
        if (!l)
        {
            DE_ERROR("Got nullptr")
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);
        m_listeners.erase(
            std::remove(m_listeners.begin(), m_listeners.end(), l),
            m_listeners.end()
        );
    }

    void postMessage(f64 pts, const ShortMidiMessage& msg)
    {
        /*
        if ( m_midiOut )
        {
            m_midiOut->sendShortMessage( msg.pack() );
        }
        */
        int errorCount = 0;
        int errorIndex = -1;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (size_t i = 0; i < m_listeners.size(); i++)
            {
                auto listener = m_listeners[i];
                if (!listener)
                {
                    errorCount++;
                    errorIndex = i;
                    continue;
                }
                listener->onShortMidiMessage(pts, msg);
            }
        }
        if (errorCount > 0)
        {
            DE_ERROR("errorCount = ",errorCount,", errorIndex = ",errorIndex)
        }
    }

    void postMessage(f64 pts, const MidiMessage & msg)
    {
        /*
        if ( m_midiOut )
        {
            m_midiOutBuffer.clear();
            m_midiOutBuffer.assign( msg.begin(), msg.end() );
            m_midiOut->sendMessage( &m_midiOutBuffer );
        }

        */
        DE_DEBUG("MidiMessage.size = ", msg.size())

        int errorCount = 0;
        int errorIndex = -1;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (size_t i = 0; i < m_listeners.size(); ++i)
            {
                auto listener = m_listeners[i];
                if (!listener)
                {
                    errorCount++;
                    errorIndex = i;
                    continue;
                }
                listener->onMidiMessage(pts, msg);
            }
        }

        if (errorCount > 0)
        {
            DE_ERROR("errorCount = ",errorCount,", errorIndex = ",errorIndex)
        }

    }

    static void
    CB_RtMidiInput( double deltaTime, std::vector< uint8_t >* message, void* userData )
    {
        if (!message || message->empty())
        {
            return;
        }

        auto central = reinterpret_cast<MidiCentral_Private*>( userData );
        if (!central)
        {
            DE_ERROR("No MidiCentral")
        }

        const int n = message->size();
        if (n <= 4)
        {
            ShortMidiMessage e;
            e.m_status = message->at(0);
            if (n > 1) e.m_data1 = message->at(1);
            if (n > 2) e.m_data2 = message->at(2);
            if (n > 3) e.m_data3 = message->at(3);

            // Filter CC LSB controller events
            uint8_t command = e.m_status & 0xF0;
            //uint8_t channel = e.status & 0x0F;

            if (command == 0xB0 && e.m_data1 > 31 && e.m_data1 < 64)
            {
                DE_WARN("Filtered CC LSB controller event ", e.str())
            }
            else
            {
                central->postMessage( deltaTime, e );
            }
        }
        else
        {
            DE_ERROR("Unsupported")
            // central->postMessage( deltaTime, *message );
        }
    }

    void openInput( int portIndex )
    {
        closeInput();
        m_midiIn = MidiUtil::openMidiIn( portIndex );
        if (m_midiIn)
        {
            m_portIn = portIndex;
            m_midiIn->ignoreTypes(false, false, false);
            m_midiIn->setCallback(&CB_RtMidiInput, this);
        }
    }

    void closeInput()
    {
        if ( m_midiIn )
        {
            delete m_midiIn;
            m_midiIn = nullptr;
        }
    }

    void openOutput( int portIndex )
    {
        closeOutput();
        m_midiOut = MidiUtil::openMidiOut( portIndex );
        if (m_midiOut)
        {
            DE_TRACE("")
            m_portOut = portIndex;
        }
        else
        {
            DE_ERROR("")
        }
    }

    void closeOutput()
    {
        if ( m_midiOut )
        {
            DE_TRACE("")
            delete m_midiOut;
            m_midiOut = nullptr;
        }
    }
};

// =======================================================
MidiCentral::MidiCentral()
// =======================================================
    : _d(new MidiCentral_Private)
{

}

MidiCentral::~MidiCentral()
{
    delete _d;
}

void MidiCentral::setup()
{
    _d->setup();
}

void MidiCentral::close()
{
    _d->close();
}

void MidiCentral::registerListener(IMidiMessageListener* l)
{
    _d->registerListener(l);
}

void MidiCentral::deregisterListener(IMidiMessageListener* l)
{
    _d->deregisterListener(l);
}

void MidiCentral::postMessage(f64 pts, const ShortMidiMessage& msg)
{
    _d->postMessage(pts, msg);
}

void MidiCentral::postMessage(f64 pts, const MidiMessage & msg)
{
    _d->postMessage(pts, msg);
}

// void sendMessage( const std::vector< uint8_t > & message );
// void sendShortMessage( int command, int channel, int midiNote, int velocity );
// void sendNoteOn( int channel = 0, int midiNote = 69, int velocity = 127 );
// void sendNoteOff( int channel = 0, int midiNote = 69, int velocity = 127 );

} // end namespace midi.
} // end namespace de.

/*
    if ( byteCount == 3 )
    {
        u8 status = buffer[0];
        u8 command = status & 0xF0;
        u8 channel = status & 0x0F;
        u8 data1 = buffer[1];   // midiNote, cc
        u8 data2 = buffer[2];   // velocity, value

        if ( command == 0x80 ) // NoteOff
        {
            amsynth_short_midi_event_t sme;
            sme.channel = channel;
            sme.command = command;
            sme.midiNote = data1;
            sme.velocity = data2;

            driver->m_midiInBuffer.emplace_back( std::move(sme) );
        }
        else if ( command == 0x90 ) // NoteOn
        {
            amsynth_short_midi_event_t sme;
            sme.channel = channel;
            sme.command = command;
            sme.midiNote = data1;
            sme.velocity = data2;

            driver->m_midiInBuffer.emplace_back( std::move(sme) );
        }
        else if ( command == 0xB0 ) // ControlChange
        {
            amsynth_short_midi_event_t sme;
            sme.channel = channel;
            sme.command = command;
            sme.midiNote = data1;
            sme.velocity = data2;

            driver->m_midiInBuffer.emplace_back( std::move(sme) );
        }
    }
*/
