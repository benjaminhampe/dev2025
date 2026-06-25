#pragma once
#include <QWidget>
#include <QStackedWidget>
#include "gui/track/TrackWidget.h"
#include "gui/track/QuickHelp.h"
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
    void applySkin();
    void updateLayout();
    void setAudioOnly(bool bAudioOnly);
    void showQuickHelp(bool bVisible);

    void setTrackWidget(TrackWidget* trackWidget);
    const TrackWidget* getTrackWidget() const { return m_trackWidget; }
    TrackWidget* getTrackWidget() { return m_trackWidget; }

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
private:
    QuickHelp* m_quickHelp;
    MidiMeter* m_midiMeter;
    AudioMeter* m_audioMeter;
    TrackWidget* m_trackWidget;

    //bool m_bShowQuickHelp = false; // false = MidiMeter visible, AudioMeter invisible.
    bool m_bAudioOnly = false; // false = MidiMeter visible, AudioMeter invisible.
    bool m_bFocused = false;
    bool m_bHovered = false;

    int m_zoom = 100;
    //int m_baseQuickHelpWidth = 300;
    //int m_lastQuickHelpWidth = 300;
    int m_quickHelpWidth = 300;
    //int m_baseWidth = 284;
    //int m_baseHeight = 376 + 10;

    int m_baseMargin = 4;
    int m_baseRadius = 10;

    //int m_height;
    int m_margin;
    int m_radius;

    QColor m_windowColor;

};
