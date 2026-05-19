#pragma once
#include <de/file/mp4/MP4-Atoms-Tree.h>
#include <de/file/mp4/MP4-AudioFile.h>
#include <de/file/mp4/MP4-AudioFileParsing.h>

#include <de/file/mp4/MP4-Streaming.h>
#include <de/file/mp4/IParserListener.h>

#include <de/file/mp4/Atom.h>
#include <de/file/mp4/ftyp/Atom_ftyp.h>
#include <de/file/mp4/moov/Atom_moov.h>

/*
✅ Minimale Open‑Source‑Decoder‑Kombination für alle mp4a‑Varianten
1. FAAD2 — AAC LC/HE/HEv2/LD/ELD

Dekodiert:
    AAC LC
    AAC Main
    AAC LTP
    AAC SSR
    HE‑AAC (SBR)
    HE‑AAC v2 (PS)
    AAC LD / ELD (teilweise)

Warum nötig:
Der einzige freie AAC‑Decoder, der alle Profile halbwegs vollständig kann.
2. FFmpeg/libavcodec — fallback für exotische MPEG‑4‑Audio‑Objekte

Dekodiert zusätzlich:

    TwinVQ

    CELP

    HVXC

    BSAC

    MPEG‑4 ALS (über eigenen Decoder)

    MPEG‑4 SAOL (teilweise)

    MPEG‑4 Speech Tools

    MP3 (falls in mp4a verpackt)

Warum nötig:
Nur FFmpeg deckt die obskuren MPEG‑4‑Audio‑Objekte ab, die FAAD2 nicht kann.
3. ALAC Decoder (Apple Lossless) — für ALAC in mp4a

Dekodiert:

    ALAC (Apple Lossless)

Warum nötig:
ALAC war historisch unter mp4a eingetragen, bevor Apple alac als eigenen SampleEntry definierte.
Du wirst Dateien finden, wo ALAC unter mp4a + esds steckt.

Open‑Source‑Implementationen:

    FFmpeg ALAC Decoder

    Apple Open Source ALAC Decoder (C++)

4. libmp3lame / mpg123 — MP3 in mp4a

    Dekodiert:

        MP3 (MPEG‑1/2 Layer III)

    Warum nötig:
    MP3 kann legal in MP4 vorkommen (objectTypeIndication = 0x6B).
    FAAD2 kann kein MP3.

🔥 Damit deckst du jede mögliche mp4a‑Variante ab
    MPEG‑4 Audio Codec	objectTypeIndication	Decoder
    AAC LC/HE/HEv2/LD/ELD	0x40	FAAD2
    ALS	0xA5	FFmpeg
    TwinVQ	0xC2	FFmpeg
    CELP	0xC0	FFmpeg
    HVXC	0xC1	FFmpeg
    SAOL	0xC5	FFmpeg
    BSAC	0xC3	FFmpeg
    MP3	0x6B	mpg123 / FFmpeg
    ALAC	0xA9 (oder esds)	ALAC Decoder / FFmpeg
*/

namespace de {
namespace file {
namespace mp4 {

enum eLibSupport
{
    FAAD2 = 0,
    ALAC,
};

struct MP4
{
    std::string m_uri;
    Atom m_root;

    // std::vector<Atom>       m_atoms; // All atoms (not sure yet for what, maybe debugging)

    // ✔ Level 0 (TopLevel)
    std::vector<Atom_ftyp>  m_ftyp; // normal MP4
    std::vector<Atom>       m_styp; // fragmented MP4
    std::vector<Atom>       m_free; // padding
    std::vector<Atom>       m_skip; // padding
    std::vector<Atom>       m_uuid; // user‑defined box
    std::vector<Atom_moov>  m_moov; // movie metadata
    std::vector<Atom>       m_mdat; // media data
    std::vector<Atom>       m_meta; // metadata container

    // ✔ Level 1-10
    std::vector<const Atom_trak*>  m_trak;
    // std::vector<Atom_stsz>  m_stsz;
    // std::vector<Atom_stz2>  m_stz2;
    // std::vector<Atom_stco>  m_stco;
    // std::vector<Atom_co64>  m_co64;
    // std::vector<Atom_mp4a>  m_mp4a; // AAC

/*
    void collect_trak()
    {
        for (const Atom_moov & moov : m_moov)
        {
            for (const Atom_trak & trak : moov.m_trak)
            {
                m_trak.emplace_back(&trak);
            }
        }
    }

    void collect_trak_audio()
    {
        for (const Atom_moov & moov : m_moov)
        {
            for (const Atom_trak & trak : moov.m_trak)
            {
                for (const Atom_mdia & mdia : trak.m_mdia)
                {
                    for (const Atom_hdlr & hdlr : mdia.m_hdlr)
                    {
                        if (hdlr.media_type() == "soun")
                        {
                            m_trak.emplace_back(&trak);
                        }
                    }
                }
            }
        }
    }
*/

    struct TrackByLibSupport
    {
        const Atom_trak* trak = nullptr;
        const Atom_esds* esds = nullptr;
        std::vector<const Atom_stbl*> stbls;
    };

    TrackByLibSupport getTrackByLibSupport(eLibSupport libSupport) const
    {
        TrackByLibSupport result;

        std::vector<const Atom_trak*> tracks;

        for (const Atom_moov & moov : m_moov)
        {
            for (const Atom_trak & trak : moov.m_trak)
            {
                for (const Atom_mdia & mdia : trak.m_mdia)
                {
                    for (const Atom_hdlr & hdlr : mdia.m_hdlr)
                    {
                        if (hdlr.media_type() == "soun")
                        {
                            tracks.emplace_back(&trak);
                        }
                    }
                }
            }
        }

        DE_BENNI("Found ", tracks.size(), " audio track candidates.")

        if (tracks.empty()) return result;

        result.trak = tracks.front();

        std::vector<const Atom_stbl*> stbls;

        // AAC LC
        // AAC Main
        // AAC LTP
        // AAC SSR
        // HE‑AAC (SBR)
        // HE‑AAC v2 (PS)
        // AAC LD / ELD (teilweise)

        for (const Atom_trak* trak : tracks)
        {
            for (const Atom_mdia & mdia : trak->m_mdia)
            {
                for (const Atom_minf & minf : mdia.m_minf)
                {
                    for (const Atom_stbl & stbl : minf.m_stbl)
                    {
                        for (const Atom_stsd & stsd : stbl.m_stsd)
                        {
                            for (const Atom_mp4a & mp4a : stsd.m_mp4a)
                            {
                                if (mp4a.m_esds)
                                {
                                    const Atom_esds & esds = *mp4a.m_esds;

                                    // 🔥 Damit deckst du jede mögliche mp4a‑Variante ab
                                    // MPEG‑4 Audio Codec	objectTypeIndication	Decoder
                                    // AAC LC/HE/HEv2/LD/ELD	0x40                FAAD2
                                    // ALS                      0xA5                FFmpeg
                                    // TwinVQ                   0xC2                FFmpeg
                                    // CELP                     0xC0                FFmpeg
                                    // HVXC                     0xC1                FFmpeg
                                    // SAOL                     0xC5                FFmpeg
                                    // BSAC                     0xC3                FFmpeg
                                    // MP3                      0x6B                mpg123 / FFmpeg
                                    // ALAC                     0xA9 (oder esds)	ALAC Decoder / FFmpeg

                                    const uint8_t oti = esds.m_esd.decConfig.objectTypeIndication;
                                    switch (libSupport)
                                    {
                                        case FAAD2:
                                            if ((oti == 0x40) || (oti == 0x66) || (oti == 0x67) || (oti == 0x68))
                                            {
                                                result.esds = &esds;
                                                stbls.emplace_back(&stbl);
                                            } break;
                                        case ALAC:
                                            if (oti == 0xA9)
                                            {
                                                result.esds = &esds;
                                                stbls.emplace_back(&stbl);
                                            } break;
                                        default:
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        DE_BENNI("Found ", stbls.size(), " stbls your lib can decode.")

        if (stbls.empty()) return result;

        result.stbls = stbls;

        return result;
    }


    std::string str() const
    {
        std::ostringstream o;

        DE_OK("ftyp = ", m_ftyp.size())
        DE_OK("moov = ", m_moov.size())
        DE_OK("mdat = ", m_mdat.size())
        DE_OK("audio_trak = ", m_trak.size())

        return o.str();
    }

};

// ============================================================================
struct Parser
// ============================================================================
{
    static bool parse( const std::string & uri, MP4 & mp4 );
};

#if 0

// ============================================================================
struct Parser : public IParserListener
// ============================================================================
{
    std::vector< IParserListener* > m_listeners; // One and only member
    File m_file;



    Parser();

    bool
    parse( const std::string & uri );

    void
    parse();

    /*

    // Must be atleast 8 bytes.
    std::optional<Atom>
    parse_toplevel_atom( uint8_t const* const beg, uint8_t const* const end );


    bool
    parse_ftyp( Atom_ftyp & atom );


    size_t
    parse( uint8_t const* const beg, uint8_t const* const end, const std::string & uri );


    // Must always return 14 (bytes).
    size_t
    parseFileHeader( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseMessage( uint8_t const* const beg, uint8_t const* const end );

    // Must always return 8 (bytes).
    // Some files dont have TrackHeaders
    size_t
    parseTrackHeader( uint8_t const* const beg, uint8_t const* const end, uint32_t & dataSize );

    size_t
    parseTrackEnd( uint8_t const* const beg, uint8_t const* const end );

    //size_t
    //parseTrackData( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseEvent( uint8_t const* const beg, uint8_t const* const end );

    size_t
    parseMetaEvent(
    uint8_t const* const vt, // Event start ( time ), needed for full token extract
    uint8_t const* const beg, // Current pos (after reading byte metatype).
    uint8_t const* const end, // End of string
    uint32_t deltaTime, // Already parsed dt injected here
    uint8_t metatype    // Already parsed metatype injected here
    );

    std::string
    parseMetaText( uint8_t const* const beg, uint8_t const* const end );

*/
    // =======================================================================
    //struct MidiParserListenerRegistry
    // =======================================================================


    // ~MidiParserListenerRegistry() {}

    // Removes all listeners at once.
    // Simpler than having a removeListener() function.
    void clearListeners()
    {
        m_listeners.clear();
    }

    // Adds/registers a listener, rejects null pointers.
    void addListener( IParserListener* listener )
    {
        if ( !listener ) return;
        m_listeners.emplace_back( listener );
    }

    // I.:

    void onParserStart( uint8_t const* beg, uint8_t const* end, std::string const & uri )
    {
        for (auto l : m_listeners) { if (l) l->onParserStart( beg, end, uri ); }
    }

    void onParserEnd()
    {
        for (auto l : m_listeners) { if (l) l->onParserEnd(); }
    }

    void onAtom(uint64_t parent,
                uint64_t atomId,
                const uint8_t* atomBeg,
                const uint8_t* atomEnd,
                const std::string & utf8_atomName,
                const uint64_t dataSize,
                const uint8_t* dataBeg,
                const uint8_t* dataEnd )
    {
        for (auto l : m_listeners)
        {
            if (l) l->onAtom(   parent,
                                atomId,
                                atomBeg,
                                atomEnd,
                                utf8_atomName,
                                dataSize,
                                dataBeg,
                                dataEnd );
        }
    }

/*
    void mpTrack( uint8_t const* beg, uint8_t const* end, int trackNumber )
    {
    for (auto l : m_listeners) { if (l) l->mpTrack( beg, end, trackNumber ); }
    }

    // II.:

    void mpFileHeader( int fileType, int trackCount, int ticksPerQuarterNote )
    {
    for (auto l : m_listeners) { if (l) l->mpFileHeader( fileType, trackCount, ticksPerQuarterNote ); }
    }
    void mpTrackStart( int trackIndex )
    {
    for (auto l : m_listeners) { if (l) l->mpTrackStart( trackIndex ); }
    }
    void mpTrackEnd()
    {
    for (auto l : m_listeners) { if (l) l->mpTrackEnd(); }
    }

    // III.:

    void mpNoteOn( uint64_t tick, int channel, int midiNote, int velocity )
    {
    for (auto l : m_listeners) { if (l) l->mpNoteOn( tick, channel, midiNote, velocity ); }
    }
    void mpNoteOff( uint64_t tick, int channel, int midiNote, int velocity )
    {
    for (auto l : m_listeners) { if (l) l->mpNoteOff( tick, channel, midiNote, velocity ); }
    }
    void mpPolyphonicAftertouch( uint64_t tick, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpPolyphonicAftertouch( tick, value ); }
    }
    void mpChannelAftertouch( uint64_t tick, int channel, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpChannelAftertouch( tick, channel, value ); }
    }
    void mpPitchBend( uint64_t tick, int channel, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpPitchBend( tick, channel, value ); }
    }
    void mpProgramChange( uint64_t tick, int channel, int program )
    {
    for (auto l : m_listeners) { if (l) l->mpProgramChange( tick, channel, program ); }
    }
    void mpControlChange( uint64_t tick, int channel, int cc, int value )
    {
    for (auto l : m_listeners) { if (l) l->mpControlChange( tick, channel, cc, value ); }
    }
    void mpSetTempo( uint64_t tick, float beatsPerMinute, int microsecondsPerTick )
    {
    for (auto l : m_listeners) { if (l) l->mpSetTempo( tick, beatsPerMinute, microsecondsPerTick ); }
    }
    void mpSMPTEOffset( uint64_t tick, int hh, int mm, int ss, int fc, int sf )
    {
    for (auto l : m_listeners) { if (l) l->mpSMPTEOffset( tick, hh, mm, ss, fc, sf ); }
    }
    void mpTimeSignature( uint64_t tick, int top, int bottom, int clocksPerBeat, int n32rd_per_beat )
    {
    for (auto l : m_listeners) { if (l) l->mpTimeSignature( tick, top, bottom, clocksPerBeat, n32rd_per_beat ); }
    }
    void mpKeySignature( uint64_t tick, int tonart_c_offset, int minor )
    {
    for (auto l : m_listeners) { if (l) l->mpKeySignature( tick, tonart_c_offset, minor ); }
    }
    void mpSequenceNumber( uint64_t tick, int sequenceNumber )
    {
    for (auto l : m_listeners) { if (l) l->mpSequenceNumber( tick, sequenceNumber ); }
    }
    void mpChannelPrefix( uint64_t tick, int channelPrefix )
    {
    for (auto l : m_listeners) { if (l) l->mpChannelPrefix( tick, channelPrefix ); }
    }
    void mpPortDisplay( uint64_t tick, int port )
    {
    for (auto l : m_listeners) { if (l) l->mpPortDisplay( tick, port ); }
    }
    void mpMeta( uint64_t tick, int metaType, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMeta( tick, metaType, metaText ); }
    }
    void mpMetaText( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaText( tick, metaText ); }
    }
    void mpMetaCopyright( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaCopyright( tick, metaText ); }
    }
    void mpMetaTrackName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaTrackName( tick, metaText ); }
    }
    void mpMetaInstrumentName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaInstrumentName( tick, metaText ); }
    }
    void mpMetaLyric( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaLyric( tick, metaText ); }
    }
    void mpMetaMarker( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaMarker( tick, metaText ); }
    }
    void mpMetaCuePoint( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaCuePoint( tick, metaText ); }
    }
    void mpMetaProgramName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaProgramName( tick, metaText ); }
    }
    void mpMetaDeviceName( uint64_t tick, std::string const & metaText )
    {
    for (auto l : m_listeners) { if (l) l->mpMetaDeviceName( tick, metaText ); }
    }
    */
};

#endif

} // end namespace mp4.
} // end namespace file.
} // end namespace de.
