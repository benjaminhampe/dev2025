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

#include "gui/footer/PixButton.h"
// #include "gui/footer/TrackOverview.h"
// #include "gui/footer/ClipOverview.h"
// #include "gui/footer/LongText.h"

// ============================================================================
class Footer : public QWidget
// ============================================================================
{
    Q_OBJECT
    bool m_hasFocus;
    //bool m_isQuickHelpPanelVisible;
    //bool m_isMidiKeyboardVisible;
    //bool m_isDetailPanelVisible;

    PixButton* m_btnQuickHelp;
    //LongText* m_longText;
    PixButton* m_btnMidiKeyboard;
    PixButton* m_btnClipOverview;
    PixButton* m_btnTrackOverview;
    PixButton* m_btnDetails;

    QString m_longText;

    int m_baseRadius = 8;
    int m_basePadding = 8;
    int m_baseButtonHeight = 48;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;

    int m_radius;
    int m_padding;
    int m_buttonHeight;

    QRect m_rcLongText;

    QPixmap m_trackOverviewPixmap;
    int m_trackOverviewVisibleWidth;
    int m_trackOverviewTotalWidth;
    int m_trackOverviewPos;
    // Footer Contents, Computed
    // QRect m_rcFooterContent;
    // QRect m_rcBtnShowQuickHelpPanel;
    // QRect m_rcLongPanel;
    // QRect m_rcLongView;
    // QRect m_rcBtnShowMidiKeyboardPanel;
    // QRect m_rcClipOverviewPanel;
    // QRect m_rcClipOverview;
    // QRect m_rcTrackOverviewPanel;
    // QRect m_rcTrackOverview;
    // QRect m_rcBtnShowDetailPanel; // Computed
public:
    Footer(QWidget* parent = 0);
    ~Footer() override;

    bool hasFocus() const { return m_hasFocus; }

    void applySkin();
    void updateLayout();

    void setTrackOverview(QPixmap pix, int visibleWidth, int totalWidth, int xPos);
signals:
public slots:
    //void setTextSpurOverview( QString txt ) { m_spurText = txt; updateLayout(); }
protected slots:
    void on_btnShowMidiKeyboard( bool checked );
    void on_btnShowQuickHelpPanel( bool checked );
    void on_btnShowDetailPanel( bool checked );
    //void on_currentTrackIdChanged( int index );
    void on_btnShowClipOverview( bool checked );
    void on_btnShowTrackOverview( bool checked );

protected:
    void resizeEvent( QResizeEvent* event ) override;
    void paintEvent( QPaintEvent* event ) override;
    void focusInEvent( QFocusEvent* event ) override;
    void focusOutEvent( QFocusEvent* event ) override;
/*
    void enterEvent( QEnterEvent* event ) override;
    void leaveEvent( QEvent* event ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseReleaseEvent( QMouseEvent* event ) override;
    void mouseMoveEvent( QMouseEvent* event ) override;
    void wheelEvent( QWheelEvent* event ) override;
    void keyPressEvent( QKeyEvent* event ) override;
    void keyReleaseEvent( QKeyEvent* event ) override;
*/
private:
/*
    ImageButton* createShowQuickHelpPanelButton();
    ImageButton* createShowMidiKeyboardButton();
    ImageButton* createShowDetailPanelButton();
*/
};
