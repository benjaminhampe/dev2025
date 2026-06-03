#pragma once
#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>
#include "gui/track/Track.h"
#include "gui/track/details/AudioMeter.h"
#include "gui/track/details/MidiMeter.h"

// ============================================
class ChainWrapper : public QWidget
// ============================================
{
    Q_OBJECT
public:
    ChainWrapper(QWidget* parent = nullptr);
    ~ChainWrapper() override;
    void applySkin();
    void setAudioOnly(bool bAudioOnly);

    const Track* trackWidget() const { return m_track; }
    Track* trackWidget() { return m_track; }

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
    MidiMeter* m_midiMeter;
    AudioMeter* m_audioMeter;
    QScrollArea* m_scrollArea;
    Track* m_track;

    int m_baseWidth = 284;
    int m_baseHeight = 326;

    int m_baseMargin = 4;
    int m_baseRadius = 10;

    bool m_bAudioOnly = false; // false = MidiMeter visible, AudioMeter invisible.
    bool m_bFocused = false;
    bool m_bHovered = false;

    QColor m_windowColor;
    int m_margin;
    int m_radius;
};
