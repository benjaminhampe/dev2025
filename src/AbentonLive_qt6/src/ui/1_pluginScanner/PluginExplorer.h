#pragma once
#include "PluginScanner.h"
#include "PluginInfoTree.h"
#include <QListWidget>
#include "ImageButton.h"
struct App;

// ============================================================================
class PluginExplorer : public QWidget
// ============================================================================
{
   Q_OBJECT
   App & m_app;

   QRect m_clipRect;
   QRect m_rcPluginScanner; // Computed, buttons
   QRect m_rcPluginInfoTree; // Computed
   QRect m_rcSearchBar; // Computed
   QRect m_rcBtnMore;

//   int m_searchH = 20; // <searchBar> height
//   int m_thH = 10;   // <th> height
//   int m_tdH = 16;   // <td> height
//   int m_tdB = 1;    // <td> border
//   int m_iX = 10;// <td> icon offset x
//   int m_iY = 10;// <td> icon offset y
//   int m_nX = 10;// <td> name text offset x
//   int m_nY = 10;// <td> name text offset y
public:
   PluginExplorer( App & app, QWidget* parent = 0 );
   ~PluginExplorer() override {}
signals:
public slots:
   void updateLayout();
protected slots:
   void on_btnShowPluginDirs( bool checked );
protected:
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
//   void enterEvent( QEvent* event ) override;
//   void leaveEvent( QEvent* event ) override;
//   void mousePressEvent( QMouseEvent* event ) override;
//   void mouseReleaseEvent( QMouseEvent* event ) override;
//   void mouseMoveEvent( QMouseEvent* event ) override;
//   void wheelEvent( QWheelEvent* event );
protected:
   ImageButton* createShowMoreButton();

};
