#pragma once
#include <QTreeWidget>
#include "LiveSkin.h"

struct App;

// ============================================================================
class PluginDirTree : public QTreeWidget
// ============================================================================
{
   Q_OBJECT
public:
   PluginDirTree( App & app, QWidget* parent = 0 );
   ~PluginDirTree() override {}
signals:
public slots:
   void createLiveTreeHeader();
   void populateFromPluginDb();
protected slots:
   void on_itemPressed( QTreeWidgetItem*, int );
protected:

   App & m_app;
   QTreeWidgetItem* m_selected;
};
