#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>

// ============================================================================
class Footer : public QWidget
// ============================================================================
{
    Q_OBJECT
public:
    bool m_bHelpVisible{ false };
    bool m_bMidiVisible{ false };
    bool m_bClipVisible{ false };
    bool m_bTrackVisible{ true };
    bool m_bArraVisible{ false };

    struct QuickHelp
    {
        QRect rc;
        QPixmap on;
        QPixmap off;
    };
    QuickHelp m_quickHelp;

    struct LongText
    {
        QRect rc;
        QString text;
    };
    LongText m_longText;

    struct MidiKeyboard
    {
        QRect rc;
        QPixmap on;
        QPixmap off;
    };
    MidiKeyboard m_midiKeyboard;

    struct ClipShow
    {
        QRect rc;
    };
    QPixmap m_arrowUp;
    QPixmap m_arrowRight;

    ClipShow m_clipShow;

    struct ClipName
    {
        QRect rc;
        QPixmap pix;
        QString text;
    };
    ClipName m_clipName;

    struct ClipScroll
    {
        QRect rc;
        QRect rcOverview;
        QPixmap pix;
        int view = 100; // Scrollbar
        int total = 100; // Scrollbar
        int pos = 0; // Scrollbar
        int borderRadius = 8;
        int borderWidth = 4;
        QColor borderColor = QColor(32,32,32);
    };
    ClipScroll m_clipScroll;

    struct TrackShow
    {
        QRect rc;
    };
    TrackShow m_trackShow;

    struct TrackName
    {
        QRect rc;
        QPixmap pix;
        QString text;
    };
    TrackName m_trackName;

    struct TrackScroll
    {
        QRect rc;
        QRect rcOverview;
        QPixmap pix;
        int view = 100; // Scrollbar
        int total = 100; // Scrollbar
        int pos = 0; // Scrollbar
        int borderRadius = 8;
        int borderWidth = 4;
        QColor borderColor = QColor(32,32,32);
    };
    TrackScroll m_trackScroll;

    struct Details
    {
        QRect rc;
        QPixmap on;
        QPixmap off;
    };
    Details m_details;

    // Skin
    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QColor m_activeColor;
    QFont m_helpFont;

    int m_zoom = 100;
    int m_radius;
    int m_padding;
    int m_buttonHeight;

public:
    Footer(QWidget* parent = 0);
    void applySkin();
    void updateLayout();
    int computeBestHeight() const;
signals:
    void sig_showQuickHelp(bool bVisible);
    void sig_showMidiKeyboard(bool bVisible);
    void sig_showClipEditor(bool bVisible);
    void sig_showTrackEditor(bool bVisible);
    void sig_showArrangement(bool bVisible);
public slots:
    void setTrackOverview(QPixmap pix, int visibleWidth, int totalWidth, int xPos);
    void setTrackName(QString name);
    void setClipName(QString name);
protected slots:
    void on_showContextMenuTrack(const QPoint& pos);
    void on_showContextMenuClip(const QPoint& pos);
protected:
    bool event(QEvent* event) override;
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;

/*
    void enterEvent( QEnterEvent* event ) override;
    void leaveEvent( QEvent* event ) override;
    void focusInEvent( QFocusEvent* event ) override;
    void focusOutEvent( QFocusEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;
*/
private:

    static QPixmap
    PIX_createFromText(int padd_x, int padd_y, QString text, QColor fillColor, QColor textColor, QFont font);

    static QPixmap
    PIX_createQuickHelp(int w, int h, const QColor& fillColor, const QColor& lineColor);

    static QPixmap
    PIX_createMidiKeyboard(int w, int h, QColor whiteColor, QColor blackColor, QColor redColor, QColor blueColor);

    static QPixmap
    PIX_createArrowRight(int w, int h, int r, QColor windowColor, QColor panelColor, QColor symbolColor);

    static QPixmap
    PIX_createArrowUp(int w, int h, int r, QColor windowColor, QColor panelColor, QColor symbolColor);

    static QPixmap
    PIX_createDetails(int w, int h, QColor fillColor, QColor lineColor);

};
