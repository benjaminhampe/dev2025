#pragma once
#include "LiveSkin.h"
#include <QTreeWidget>

#ifndef USE_PLUGIN_DRAG_N_DROP
#define USE_PLUGIN_DRAG_N_DROP
#endif

#ifdef USE_PLUGIN_DRAG_N_DROP
   #include <QMimeData>
   #include <QDrag>
   #include <QDropEvent>
#endif

struct App;

// ============================================================================
class PluginInfoTree : public QTreeWidget
// ============================================================================
{
   Q_OBJECT
public:
   PluginInfoTree( App & app, QWidget* parent = 0 );
   ~PluginInfoTree() override {}
signals:
public slots:
   void populateFromPluginDb();
   void createLiveTreeHeader();
protected slots:
   void on_itemPressed( QTreeWidgetItem*, int );
protected:
   //void resizeEvent( QResizeEvent* event ) override;
   //void paintEvent( QPaintEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;

   void enterEvent( QEnterEvent* event ) override;
   void leaveEvent( QEvent* event ) override;
   void focusInEvent( QFocusEvent* event ) override;
   void focusOutEvent( QFocusEvent* event ) override;
protected:


   App & m_app;
   QTreeWidgetItem* m_synthNode;
   QTreeWidgetItem* m_effectNode;
   QTreeWidgetItem* m_selectedItem;
   bool m_isDragging;
   int m_dragStartX;
   int m_dragStartY;

//   QDrag* m_drag;

//   QRect m_clipRect;
//   de::audio::PluginInfoList m_pluginDb; // plugin database
//   QImage m_imgVSTLogo;
//   QImage m_imgSynth;
//   QImage m_imgEffect;


//   int m_listStart = 0; // Current scroll index y
//   int m_listCount = 0; // Computed at runtime, depends on curr widget y height.

//   int m_searchH = 20; // <searchBar> height
//   int m_thH = 10;   // <th> height
//   int m_tdH = 16;   // <td> height
//   int m_tdB = 1;    // <td> border
//   int m_iX = 10;// <td> icon offset x
//   int m_iY = 10;// <td> icon offset y
//   int m_nX = 10;// <td> name text offset x
//   int m_nY = 10;// <td> name text offset y
};

