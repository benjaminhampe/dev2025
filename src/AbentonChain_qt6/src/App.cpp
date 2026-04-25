#include "App.h"
#include <gui/viz/GL_Canvas.h>

// Singleton instance pointer.
App* App::m_pInstance = nullptr;

App::App(QObject* parent)
    : QObject(parent)
    , m_canvas{ nullptr }
    , m_inputDeviceId(0)
    , m_outputDeviceId(0)
    , m_blockSize(0)
    , m_channels(0)
    , m_sampleRate(0)
    , m_track(nullptr)
{
    DE_TRACE("")
    m_track = &m_track0;
}

App::~App()
{
    DE_TRACE("")
    if (m_canvas)
    {
        DE_ERROR("Canvas still active")
    }
}

App* App::instance()
{
    if (!m_pInstance)   // Only allow one instance of class to be generated.
        m_pInstance = new App;

    return m_pInstance;
}

void App::setCanvas( GL_Canvas* canvas )
{
    DE_TRACE("")
    m_canvas = canvas;
}

void App::cleanupAll()
{
    // Your cleanup before destruction
    DE_WARN("")

    stopAudio();

    if (!m_canvas)
    {
        DE_ERROR("No canvas")
    }
    else
    {
        // The renderer accesses DSP data. We need to stop that
        // before the DspChain gets deleted while the renderer
        // is still running!
        m_canvas->cleanupAll();
        DE_OK("Stop canvas rendering audio data")
        m_canvas = nullptr;
    }

    std::this_thread::sleep_for(
        std::chrono::nanoseconds(100000));

    m_dspSampleCollector.stop();

    m_track0.cleanupAll();
}

const Skin&
App::currentSkin() const
{
    return m_skinManager.current();
}

Skin&
App::currentSkin()
{
    return m_skinManager.current();
}

int App::getZoom() const
{
    return m_skinManager.current().zoom;
}

void App::setZoom(int percent)
{
    percent = std::clamp( percent, 50, 200 );

    if (m_skinManager.current().zoom != percent)
    {
        m_skinManager.current().zoom = percent;
        emit skinChanged();
    }
}

//=========================
// DriverApi
//=========================

void App::confAudio(int outputDevice, // = -1,
               int inputDevice, // = -1,
               int sampleRate, // = 48000,
               int blockSize, // = 128,
               int channels) // = 2 )
{
    //m_endPoint.start();
}

void App::playAudio()
{
    m_dspSampleCollector.dsp_setInputSignal(m_track);
    m_endPoint.setInputSignal(&m_dspSampleCollector);
    m_endPoint.play();
}

void App::stopAudio()
{
    m_endPoint.stop();
}

//=========================
// TrackApi
//=========================

int App::addTrack( std::string name )
{
    return m_track->getTrackId();
}

de::audio::Track* App::getTrack( int id )
{
    return m_track;
}

void App::removeTrack( int id )
{
    // m_track0
}

//=========================
// PluginApi
//=========================
de::audio::PluginFactory&
App::getPluginFactory() { return m_pluginFactory; }
const de::audio::PluginFactory&
App::getPluginFactory() const { return m_pluginFactory; }

//=========================
// MidiApi
//=========================
de::midi::MidiCentral&
App::getMidiCentral() { return m_midiCentral; }
const de::midi::MidiCentral&
App::getMidiCentral() const { return m_midiCentral; }

//=========================
// SampleCollector
//=========================
de::audio::DspSampleCollector&
App::getDspSampleCollector() { return m_dspSampleCollector; }
const de::audio::DspSampleCollector&
App::getDspSampleCollector() const { return m_dspSampleCollector; }
