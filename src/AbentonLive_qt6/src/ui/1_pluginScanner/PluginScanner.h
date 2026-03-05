#pragma once
#include <QListWidget>
#include "ImageButton.h"
#include "PluginDirTree.h"
#include <QTextEdit>

struct App;

// ============================================================================
class PluginScanner : public QWidget
// ============================================================================
{
   Q_OBJECT
   App & m_app;
   //ImageButton* m_btnUpdateFromDb;
   ImageButton* m_btnAddDir;
   ImageButton* m_btnDelDir;
   ImageButton* m_btnShowPluginXml;
   ImageButton* m_btnShowNetworkXml;
   ImageButton* m_btnRescan;
   ImageButton* m_btnClearDb;

   QTextEdit* m_textEdit;

public:
   PluginScanner( App & app, QWidget* parent = 0 );
   ~PluginScanner() override;
signals:
public slots:
   void scanDir( QString vstDir, bool recursive );

protected slots:
   //void on_btnUpdateFromDb( bool checked );
   void on_btnAddDir( bool checked );
   void on_btnDelDir( bool checked );
   void on_btnShowPluginXml( bool checked );
   void on_btnShowNetworkXml( bool checked );
   void on_btnRescanAll( bool checked );
   void on_btnRebuildDb( bool checked );

protected:
   //void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

protected:
   ImageButton* createUpdateFromDbButton();
   ImageButton* createButtonAddDir();
   ImageButton* createButtonDelDir();
//   ImageButton* createDirUpButton();
//   ImageButton* createDirDownButton();
   ImageButton* createShowPluginXmlButton();
   ImageButton* createShowNetworkXmlButton();
   ImageButton* createRescanAllButton();
   ImageButton* createRebuildDbButton();
   //void on_itemActivated( QTreeWidgetItem*, int );
protected:

};
