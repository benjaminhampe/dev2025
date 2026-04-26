#pragma once
#include "gui/SkinManager.h"
#include <QObject>
#include <QColor>
#include <de/midi/MidiCentral.h>
#include <de/audio/Track.h>
#include <de/audio/plugin/PluginFactory.h>
#include <de/audio/dsp/DspSampleCollector.h>
//#include <de/audio/device/EndPoint_RtAudio.h>
#include <de/audio/device/EndPoint_Wasapi.h>

class GL_Canvas;

// ============================================
class App : public QObject
// ============================================
{
    Q_OBJECT

public:
    SkinManager m_skinManager;
    GL_Canvas* m_canvas;
    // AudioCentral
    int m_inputDeviceId;
    int m_outputDeviceId;
    int m_blockSize;
    int m_channels;
    int m_sampleRate;
    de::audio::PluginFactory m_pluginFactory;
    de::midi::MidiCentral m_midiCentral;
    de::audio::Track m_track0;
    de::audio::DspSampleCollector m_sampleCollector;
    de::audio::EndPoint_Wasapi m_endPoint;
    bool m_deviceGuardFlag;
    //de::audio::EndPoint_RtAudio m_endPoint;
    de::audio::Track* m_track;
public:
    App(QObject* parent = nullptr);
    ~App();
    void cleanupAll();

    //=========================
    // SkinApi
    //=========================
    static std::shared_ptr<App> instance();
    const Skin& currentSkin() const;
    Skin& currentSkin();
    int getZoom() const;

    //=========================
    // CanvasApi
    //=========================

    void setCanvas( GL_Canvas* canvas );

    //=========================
    // DriverApi
    //=========================

    void confAudio( int outputDevice = -1,
                    int inputDevice = -1,
                    int sampleRate = 48000,
                    int blockSize = 128,
                    int channels = 2 );

    void playAudio();

    void stopAudio();

    //=========================
    // MidiApi
    //=========================
    de::midi::MidiCentral& getMidiCentral();
    const de::midi::MidiCentral& getMidiCentral() const;

    //=========================
    // SampleCollector
    //=========================
    de::audio::DspSampleCollector* getSampleCollector();
    const de::audio::DspSampleCollector* getSampleCollector() const;

    //=========================
    // PluginApi
    //=========================
    de::audio::PluginFactory& getPluginFactory();
    const de::audio::PluginFactory& getPluginFactory() const;

    //=========================
    // TrackApi
    //=========================
    int addTrack( std::string name );
    void removeTrack( int id );
    de::audio::Track* getTrack( int id );

protected:
public slots:
    void setZoom(int percent);

private slots:
    void onAudioDeviceLost();

signals:
    void skinChanged();

private:
    static std::shared_ptr<App> m_pInstance;


};
