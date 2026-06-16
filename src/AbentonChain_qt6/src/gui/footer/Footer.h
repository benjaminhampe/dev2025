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
// #include "gui/footer/TrackOverviewButton.h"
// #include "gui/footer/ClipOverview.h"
// #include "gui/footer/LongText.h"
// #include "gui/footer/FooterButton.h"

struct FooterOverview
{
    QRect rcAll;
    QRect rcBtn;
    QRect rcName;
    QRect rcPixmap; // overview

    PixButton* btnShow;
    PixButton* btnName;

    QPixmap pix; // overview acts like a scrollbar
    //QString text;
    //bool isOpen = false;

    int data = -1; // trackIndex or clipIndex when pressing on rcName

    int viewWidth; // Scrollbar
    int totalWidth;// Scrollbar
    int viewPos;   // Scrollbar

    int width() const { return btnShow->width() + 1 + btnName->width() + pix.width(); }

    void move(const int x, const int y)
    {
        int w = 0;
        int h = 0;

        btnShow->move(x+w,y);
        rcBtn = QRect(x+w,y,btnShow->width(),btnShow->height());
        w += btnShow->width() + 1;
        h = std::max(h,btnShow->height());

        btnName->move(x+w,y);
        rcName = QRect(x+w,y,btnName->width(),btnName->height());
        w += btnName->width();
        h = std::max(h,btnName->height());

        rcPixmap = QRect(x+w,y,pix.width(),pix.height());
        w += pix.width();
        h = std::max(h,pix.height());

        rcAll = QRect(x,y,w,h);
    }

    void draw(QPainter& dc)
    {
        if (pix.width() > 0 && pix.height() > 0)
        {
            dc.drawPixmap(rcPixmap.x(),rcPixmap.y(),pix);
        }
    }
};

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
    //PixButton* m_btnClipOverview;
    //PixButton* m_btnTrackOverview;

    PixButton* m_btnDetails;

    QString m_longText;

    int m_baseRadius = 8;
    int m_basePadding = 8;
    int m_baseButtonHeight = 48;

    QColor m_windowColor;
    QColor m_panelColor;
    QColor m_textColor;
    QColor m_activeColor;

    int m_radius;
    int m_padding;
    int m_buttonHeight;

    QRect m_rcLongText;

    FooterOverview m_clipOverview;
    FooterOverview m_trackOverview;

    // QRect m_rcClipName;
    // QRect m_rcTrackName;

    // QPixmap m_trackOverviewPixmap;
    // int m_trackOverviewVisibleWidth; // Scrollbar
    // int m_trackOverviewTotalWidth;   // Scrollbar
    // int m_trackOverviewPos;          // Scrollbar
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
    void showClipEditor(bool bVisible);
    void showTrackEditor(bool bVisible);
    void showQuickHelp(bool bVisible);
    void showArrangement(bool bVisible);

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
    bool event(QEvent* event) override;
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
    QPixmap createArrowRight(int w, int h,
        QColor windowColor, QColor panelColor, QColor symbolColor);

    QPixmap createArrowUp(int w, int h,
        QColor windowColor, QColor panelColor, QColor symbolColor);

    QPixmap createFromText(int w, int h, QString text,
        QColor textColor, QColor fillColor = Qt::transparent);
};
