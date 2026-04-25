#pragma once
#include "gui/SkinManager.h"
#include <QObject>
#include <QColor>
#include <de/audio/AudioCentral.h>

class GL_Canvas;

// ============================================
class App : public QObject
// ============================================
{
    Q_OBJECT
public:
    App(QObject* parent = nullptr);
    ~App();

    //void playAudio();
    void cleanupAll();


    de::audio::AudioCentral& getAudioCentral() { return m_audioCentral; }
    const de::audio::AudioCentral& getAudioCentral() const { return m_audioCentral; }
    de::midi::MidiCentral& getMidiCentral() { return m_audioCentral.getMidiCentral(); }
    const de::midi::MidiCentral& getMidiCentral() const { return m_audioCentral.getMidiCentral(); }

    static App* instance();
    const Skin& currentSkin() const;
    Skin& currentSkin();

    int getZoom() const;

    void setCanvas( GL_Canvas* canvas );
protected:
public slots:
    void setZoom(int percent);

private slots:
signals:
    void skinChanged();

private:
    static App* m_pInstance;

public:
    SkinManager m_skinManager;

    de::audio::AudioCentral m_audioCentral;

    GL_Canvas* m_canvas;
};
