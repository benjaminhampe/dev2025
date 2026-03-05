#pragma once
#include "LiveSkin.h"
#include "ArrangeItem.h"
#include "ArrangeDropTarget.h"

struct App;

// ============================================================================
class Arrangement : public QWidget
// ============================================================================
{
   DE_CREATE_LOGGER("Arrangement")
   Q_OBJECT
   App & m_app;
public:
   bool m_hasFocus;
   bool m_isHovered;

   int m_headHeight;
   int m_setHeight;
   int m_pinHeight;
   int m_trackHeight;
   int m_trackSpace;
   int m_footHeight;

   ItemV* m_selected;
   std::vector< ItemV* > m_user;
   std::vector< ItemV* > m_returns;
   ItemV* m_master;

   ArrangeDropTarget* m_dropTarget;

public:
   Arrangement( App & app, QWidget* parent = 0 );
   ~Arrangement() override {}

   ItemV* selected() { return m_selected; }

public slots:
/*
   void clearTracks();
   void updateFromTracks();
*/
   void updateLayout();
protected slots:
   //void setCurrentTrackId( selectedId );
protected:
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
//   void hideEvent( QHideEvent* event ) override;
//   void showEvent( QShowEvent* event ) override;

   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
};
