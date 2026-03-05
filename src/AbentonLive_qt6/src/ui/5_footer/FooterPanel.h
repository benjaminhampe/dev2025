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
#include "ImageButton.h"

struct App;

// ============================================================================
class FooterPanel : public QWidget
// ============================================================================
{
   Q_OBJECT
   App & m_app;
   bool m_hasFocus;
   //bool m_isQuickHelpPanelVisible;
   //bool m_isMidiKeyboardVisible;
   //bool m_isDetailPanelVisible;

   // Footer Contents, Computed
   QRect m_rcFooterContent;

   QRect m_rcBtnShowQuickHelpPanel;

   QRect m_rcLongPanel;
   QRect m_rcLongView;

   QRect m_rcBtnShowMidiKeyboardPanel;

   QRect m_rcClipOverviewPanel;
   QRect m_rcClipOverview;

   QRect m_rcTrackOverviewPanel;
   QRect m_rcTrackOverview;

   QRect m_rcBtnShowDetailPanel; // Computed
public:
   FooterPanel( App & app, QWidget* parent = 0 );
   ~FooterPanel() override {}

   bool hasFocus() const { return m_hasFocus; }

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
   void updateLayout();
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;

   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;

   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;

private:
   ImageButton* createShowQuickHelpPanelButton();
   ImageButton* createShowMidiKeyboardButton();
   ImageButton* createShowDetailPanelButton();
};
