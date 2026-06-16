#pragma once
#include <QWidget>
#include <QStackedWidget>
#include "gui/track/Track.h"
#include "gui/track/details/AudioMeter.h"
#include "gui/track/details/MidiMeter.h"

// ============================================
class TrackStack : public QWidget
// ============================================
{
    Q_OBJECT
public:
    TrackStack(QWidget* parent = nullptr);
    ~TrackStack() override;
    // QSize sizeHint() const override;
    // QSize minimumSizeHint() const override;
    void applySkin();
    void updateLayout();

    void setAudioOnly(bool bAudioOnly);

    const Track* trackWidget() const { return m_trackWidget; }
    Track* trackWidget() { return m_trackWidget; }


signals:
    void newTrackOverview(QPixmap pix); // for Footer

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void enterEvent( QEnterEvent* event ) override;
    void leaveEvent( QEvent* event ) override;

signals:
protected slots:
private slots:

private:
    QWidget* m_quickHelp;
    MidiMeter* m_midiMeter;
    AudioMeter* m_audioMeter;
    Track* m_trackWidget;

    bool m_bAudioOnly = false; // false = MidiMeter visible, AudioMeter invisible.
    bool m_bFocused = false;
    bool m_bHovered = false;

    int m_zoom = 100;
    //int m_baseWidth = 284;
    //int m_baseHeight = 376 + 10;

    int m_baseMargin = 4;
    int m_baseRadius = 10;

    //int m_height;
    int m_margin;
    int m_radius;

    QColor m_windowColor;

};
