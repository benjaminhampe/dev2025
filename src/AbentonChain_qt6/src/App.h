#pragma once
#include "gui/SkinManager.h"
#include <QObject>
#include <QColor>
#include <de/midi/MidiCentral.h>
#include <de/session/Session.h>
#include <de/audio/dsp/DspSampleCollector.h>
//#include <de/audio/device/EndPoint_RtAudio.h>
#include <de/audio/device/EndPoint_Wasapi.h>

class CentralWidget;

// ============================================
class App : public QObject
// ============================================
{
    Q_OBJECT
public:
    SkinManager m_skinManager;

    // int m_inputDeviceId;
    // int m_outputDeviceId;
    // int m_blockSize;
    // int m_channels;
    // int m_sampleRate;
    de::midi::MidiCentral m_midiCentral;

    de::audio::DspSampleCollector m_sampleCollector;
    de::audio::EndPoint_Wasapi m_endPoint;
    //bool m_deviceGuardFlag;
    //bool m_bShutdown;
    //de::audio::EndPoint_RtAudio m_endPoint;

    de::session::Session m_session;

    CentralWidget* m_centralWidget;

public:
    explicit App(QObject* parent = nullptr);
    ~App() override;

    void shutdown();

    //=========================
    // SkinApi
    //=========================
    static std::shared_ptr<App> instance();
    const Skin& currentSkin() const;
    Skin& getSkin();
    int getZoom() const;

    //=========================
    // CanvasApi
    //=========================
    // void setCanvas( GL_Canvas* canvas );

    //=========================
    // DriverApi
    //=========================
    const de::audio::IEndPoint*
    getEndPoint() const { return &m_endPoint; }
    de::audio::IEndPoint*
    getEndPoint() { return &m_endPoint; }


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
    de::audio::IPlugin* createPlugin(std::string uri);

protected:
public slots:
    void setZoom(int percent);

private slots:
    void onAudioDeviceLost();

signals:
    void skinChanged();

private:
    static std::shared_ptr<App> m_pInstance;

    static int GetFreePluginId()
    {
        static int s_id = 0;
        return ++s_id;
    }

    // std::vector< IPlugin* > m_plugins;

    bool m_bDebug = true;
    bool m_bThrowOnFail = false;

};
