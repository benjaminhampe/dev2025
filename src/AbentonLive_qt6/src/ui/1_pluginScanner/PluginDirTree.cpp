#include "PluginDirTree.h"
#include "App.h"

PluginDirTree::PluginDirTree( App & app, QWidget* parent )
   : QTreeWidget( parent )
   , m_app( app )
{
   setObjectName( "PluginDirTree" );
   setContentsMargins(0,0,0,0);
   //setAcceptDrops( true );
   setMouseTracking( true );
   createLiveTreeHeader();
   //connect( this, SIGNAL(itemActivated(QTreeWidgetItem*,int)),
   //         this, SLOT(on_itemActivated(QTreeWidgetItem*,int)) );
   connect( this, SIGNAL(itemPressed(QTreeWidgetItem*,int)),
            this, SLOT(on_itemPressed(QTreeWidgetItem*,int)) );
   populateFromPluginDb();
}

void
PluginDirTree::on_itemPressed( QTreeWidgetItem* item, int col )
{
    DE_TRACE("x:",currentIndex().column(),"; "
             "y:",currentIndex().row()," )")
   //m_selectedItem = item;
}

void
PluginDirTree::populateFromPluginDb()
{
   blockSignals( true );

   clear();
   createLiveTreeHeader();

   de::audio::PluginInfoDb & pluginDb = m_app.pluginDb();

   std::lock_guard< std::mutex > lg( pluginDb.getMutex() ); // its a lock guard, not a company.

   // All page

   QTreeWidgetItem* rootItem = nullptr;
   de::audio::PluginInfoDbDir const & dir = pluginDb.m_all;
   {
      auto item = new QTreeWidgetItem( this );  // Create widget from data
      // Icon
      int k = 0;
      item->setIcon( k, m_app.m_effectIcon );
      // Uri
      item->setText( k, "All" );
      item->setData( k, Qt::UserRole, "All" ); k++;
      // Recursive
      // item->setText( k, QString::number( int( dir.m_recursive ) ) );
      // item->setData( k, Qt::UserRole, int( dir.m_recursive ) ); k++;
      // Num MidiSynthesizer Plugins in this directory
      item->setText( k, QString::number( int( dir.m_numSynths ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numSynths ) ); k++;
      // Num AudioOnly Effect Plugins in this directory
      item->setText( k, QString::number( int( dir.m_numEffects ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numEffects ) ); k++;
      // Num Directories in this directory
      item->setText( k, QString::number( int( dir.m_numDirs ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numDirs ) ); k++;
      // Num Files in this directory
      item->setText( k, QString::number( int( dir.m_numFiles ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numFiles ) ); k++;
      // Num Bytes in this directory (FullSize of Dir)
      item->setText( k, QString::number( qulonglong(dir.m_numBytes ) ) );
      item->setData( k, Qt::UserRole, qulonglong( dir.m_numBytes ) ); k++;
      rootItem = item;
   }

   for ( de::audio::PluginInfoDbDir const * const pi : pluginDb.m_dirs ) // auto ist langweilig)
   {
      if ( !pi ) continue; // Nothing todo

      de::audio::PluginInfoDbDir const & dir = *pi; // Now give data
      auto item = new QTreeWidgetItem( rootItem );  // Create widget from data
      // Icon
      int k = 0;
      item->setIcon( k, m_app.m_effectIcon );
      // Uri
      item->setText( k, QString::fromStdWString( dir.m_uri ) );
      item->setData( k, Qt::UserRole, QString::fromStdWString( dir.m_uri ) ); k++;
      // Recursive
      // item->setText( k, QString::number( int( dir.m_recursive ) ) );
      // item->setData( k, Qt::UserRole, int( dir.m_recursive ) ); k++;
      // Num MidiSynthesizer Plugins in this directory
      item->setText( k, QString::number( int( dir.m_numSynths ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numSynths ) ); k++;
      // Num AudioOnly Effect Plugins in this directory
      item->setText( k, QString::number( int( dir.m_numEffects ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numEffects ) ); k++;
      // Num Directories in this directory
      item->setText( k, QString::number( int( dir.m_numDirs ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numDirs ) ); k++;
      // Num Files in this directory
      item->setText( k, QString::number( int( dir.m_numFiles ) ) );
      item->setData( k, Qt::UserRole, int( dir.m_numFiles ) ); k++;
      // Num Bytes in this directory (FullSize of Dir)
      item->setText( k, QString::number( qulonglong(dir.m_numBytes ) ) );
      item->setData( k, Qt::UserRole, qulonglong( dir.m_numBytes ) ); k++;
   }

   addTopLevelItem( rootItem );
   blockSignals( false );
}


void
PluginDirTree::createLiveTreeHeader()
{
   auto header = headerItem();
   int k = 0;
   header->setText( k, "Directory" ); k++;
//   header->setText( k, "Recursive" ); k++;
   header->setText( k, "Synths" ); k++;
   header->setText( k, "Effects" ); k++;

   header->setText( k, "Dirs" ); k++;
   header->setText( k, "Files" ); k++;
   header->setText( k, "Size" ); k++;
//   header->setText( k, "Used" ); k++;
//   header->setText( k, "Karat" ); k++;

   k = 0;
   setColumnWidth( k, 250 ); k++;
   //setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;

   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;
   setColumnWidth( k, 20 ); k++;

//   setColumnWidth( k, 20 ); k++;
}
