#include "PluginScanner.h"
#include "App.h"
#include "Draw.h"
#include <QProcess>
#include <QProcessEnvironment>
#include <de_fontawesome.h>

PluginScanner::PluginScanner( App & app, QWidget* parent )
   : QWidget( parent )
   , m_app( app )
{
   setObjectName( "PluginScanner" );
   setContentsMargins(0,0,0,0);
   setMouseTracking( true );
   m_app.m_pluginDirTree = new PluginDirTree( m_app, this );
   //m_btnUpdateFromDb = createUpdateFromDbButton();
   m_btnAddDir = createButtonAddDir();
   m_btnDelDir = createButtonDelDir();
   m_btnShowPluginXml = createShowPluginXmlButton();
   m_btnShowNetworkXml = createShowNetworkXmlButton();
   m_btnRescan = createRescanAllButton();
   m_btnClearDb = createRebuildDbButton();

   m_textEdit = new QTextEdit();
   m_textEdit->hide();

   auto vBtn = createVBox();
   //vBtn->addWidget( m_btnUpdateFromDb );
   vBtn->addWidget( m_btnAddDir );
   vBtn->addWidget( m_btnDelDir );
   vBtn->addWidget( m_btnShowPluginXml );
   vBtn->addWidget( m_btnShowNetworkXml );
   vBtn->addWidget( m_btnRescan );
   vBtn->addWidget( m_btnClearDb );

   auto h = createHBox();
   //h->setContentsMargins(6,6,6,6);
   h->addWidget( m_app.m_pluginDirTree );
   h->addLayout( vBtn );
   setLayout( h );

   //connect( m_btnUpdateFromDb, SIGNAL(clicked(bool)), this, SLOT(on_btnUpdateFromDb(bool)) );
   connect( m_btnAddDir, SIGNAL(clicked(bool)), this, SLOT(on_btnAddDir(bool)) );
   connect( m_btnDelDir, SIGNAL(clicked(bool)), this, SLOT(on_btnDelDir(bool)) );
   connect( m_btnShowPluginXml, SIGNAL(clicked(bool)), this, SLOT(on_btnShowPluginXml(bool)) );
   connect( m_btnShowNetworkXml, SIGNAL(clicked(bool)), this, SLOT(on_btnShowNetworkXml(bool)) );
   connect( m_btnRescan, SIGNAL(clicked(bool)), this, SLOT(on_btnRescanAll(bool)) );
   connect( m_btnClearDb, SIGNAL(clicked(bool)), this, SLOT(on_btnRebuildDb(bool)) );

}

PluginScanner::~PluginScanner()
{
   if ( m_textEdit )
   {
      delete m_textEdit;
   }
}


//void
//PluginScanner::resizeEvent( QResizeEvent* event )
//{
//   update();
//   QWidget::resizeEvent( event );
//}

//void
//PluginScanner::on_btnUpdateFromDb( bool checked )
//{
//   m_pluginDirTree->updateFromPluginDb();
//}

void
PluginScanner::on_btnAddDir( bool checked )
{
    DE_TRACE(checked)

   QFileDialog dlg(this);
   dlg.setFileMode(QFileDialog::Directory);
   //dlg.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
   dlg.setViewMode(QFileDialog::Detail);

   QStringList uris;
   if (dlg.exec())
   {
      uris = dlg.selectedFiles();
      for ( size_t i = 0; i < uris.size(); ++i )
      {
         QString uri = uris.at( i );
         scanDir( uri, true );
      }
   }
}

void
PluginScanner::on_btnDelDir( bool checked )
{
   DE_TRACE(checked)

   auto item = m_app.m_pluginDirTree->currentItem();
   if ( !item )
   {
       DE_TRACE("No item selected ",checked)
      return;
   }

   QString uri = item->text(0);

   QMessageBox msb;
   msb.setText("Do you want to remove a directory from Plugin Database?");
   msb.setInformativeText( uri );
   msb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
   msb.setDefaultButton( QMessageBox::Cancel );
   int ret = msb.exec();
   if (ret == QMessageBox::Ok)
   {
      //event->accept();
      m_app.m_pluginDb.removeDir( uri.toStdWString() );
      m_app.m_pluginDirTree->populateFromPluginDb();
      m_app.m_pluginInfoTree->populateFromPluginDb();
   }
   else if (ret == QMessageBox::Cancel)
   {
      //m_app.stopAudioMasterBlocking();
      //m_app.save( false );
      //event->ignore();
   }

}

void
PluginScanner::on_btnShowPluginXml( bool checked )
{
   m_textEdit->show();
   m_textEdit->raise();

   auto pluginXml = QString::fromStdString( m_app.m_pluginDbXml );

   QFile file( pluginXml );

   QString msg;

   if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
   {
      msg = file.readAll();
   }

   m_textEdit->setText( msg );

   QString program = "explorer.exe";
   QStringList arguments;
   std::string winDir = dbFileDir( m_app.m_pluginDbXml );
   dbStrReplace( winDir, "/", "\\");

   DE_WARN("Open explorer dir ",winDir)

   arguments << QString::fromStdString( winDir );

   QProcess* myProcess = new QProcess( this );
   myProcess->startDetached( program, arguments);
   //system( "explorer.exe /&" );
}

void
PluginScanner::on_btnShowNetworkXml( bool checked )
{
//   if ( m_app.m_trackManager )
//   {
//      m_textEdit->show();
//      m_textEdit->raise();

//      auto networkXml = QString::fromStdString( m_app.m_trackManager->uri() );

//      QFile file( networkXml );

//      QString msg;

//      if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
//      {
//         msg = file.readAll();
//      }

//      m_textEdit->setText( msg );
//   }

}

void
PluginScanner::on_btnRescanAll( bool checked )
{

}

void
PluginScanner::on_btnRebuildDb( bool checked )
{
   m_app.pluginDb().clearNoLock();
   m_app.m_pluginDirTree->populateFromPluginDb();
   m_app.m_pluginInfoTree->populateFromPluginDb();
}

void
PluginScanner::scanDir( QString vstDir, bool recursive )
{
   auto page = new de::audio::PluginInfoDbDir();
   int numSynths = 0;
   int numEffects = 0;
   int numFiles = 0;
   int numDirs = 0;
   size_t numBytes = 0;

   QDirIterator dirIt( vstDir, recursive ? QDirIterator::Subdirectories
                                         : QDirIterator::NoIteratorFlags );
   while ( dirIt.hasNext() )
   {
      QString uri = dirIt.next();
      QFileInfo fi = dirIt.fileInfo();
      if ( fi.isFile() )
      {
         auto ext = fi.suffix().toLower();
         if ( ext == "dll" )
         {
            m_app.setLongText( uri );
            try
            {
               de::audio::PluginInfo* pi = de::audio::scanPlugin( uri.toStdWString() );
               if ( pi )
               {
                  if ( pi->m_isSynth )
                  {
                     numSynths++;
                  }
                  else
                  {
                     numEffects++;
                  }

                  bool ok = page->addPlugin( pi );
                  if ( !ok )
                  {
                     delete pi;
                  }

               }


            }
            catch (...)
            {
            }

         }
         numFiles++;
         numBytes += size_t( fi.size() );
      }
      else if ( fi.isDir() )
      {
         numDirs++;
      }
   }

   page->m_uri = vstDir.toStdWString();
   page->m_recursive = recursive;
   page->m_numSynths = numSynths;
   page->m_numEffects = numEffects;
   page->m_numFiles = numFiles;
   page->m_numDirs = numDirs;
   page->m_numBytes = numBytes;


   m_app.pluginDb().addDir( page );

   DE_WARN("SAVE::")
   DE_TRACE(de_mbstr(m_app.pluginDb().toWString()))

   m_app.pluginDb().save( m_app.m_pluginDbXml );
   m_app.m_pluginDirTree->populateFromPluginDb();
   m_app.m_pluginInfoTree->populateFromPluginDb();

}

//void
//PluginScanner::resizeEvent( QResizeEvent* event )
//{
//   update();
//   QWidget::resizeEvent( event );
//}

void
PluginScanner::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      LiveSkin const & skin = m_app.m_skin;

      QPainter dc( this );
      //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

      // Draw background
      dc.fillRect( rect(), skin.contentColor );

//      // Draw title rect
//      dc.fillRect( QRect(0, 0, w-1, skin.searchBarH ), skin.titleColor );

//      // Draw table header
//      dc.fillRect( QRect(0, skin.searchBarH, w-1, skin.tdH), skin.tdColor );

//      QFont searchBarFont( "Arial Black", 10, 900, true );
//      dc.setFont( searchBarFont );
////      auto ts = dc.fontMetrics().tightBoundingRect( msg ).size();
////      int x = ( w - ts.width() ) / 2;
////      int y = ( h + ts.height() ) / 2;
////      dc.drawText( x,y,msg );
//      dc.drawText( 10,15,"VST PluginManager" );
   }

   QWidget::paintEvent( event );
}


inline QFont getPluginDirButtonFont()
{
   QFont font("FontAwesome", 11, QFont::Bold, false );
   font.setHintingPreference( QFont::PreferFullHinting );
   font.setKerning( true );
   font.setStyleStrategy( QFont::PreferAntialias );
   return font;
}

//ImageButton* createButtonAddDir();
//ImageButton* createButtonDelDir();
//ImageButton* createDirUpButton();
//ImageButton* createDirDownButton();
//ImageButton* createRescanDirButton();
//ImageButton* createRescanAllButton();
//ImageButton* createRebuildDbButton();

ImageButton*
PluginScanner::createButtonAddDir()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::plus));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
PluginScanner::createButtonDelDir()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::minus));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

/*
ImageButton*
PluginScanner::createDirUpButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(fa::arrowup);
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
PluginScanner::createDirDownButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(fa::arrowdown);
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}
*/

ImageButton*
PluginScanner::createShowPluginXmlButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::arrowup));
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
PluginScanner::createShowNetworkXmlButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::arrowdown));
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
PluginScanner::createUpdateFromDbButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::recycle));

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

ImageButton*
PluginScanner::createRescanAllButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::recycle));
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
PluginScanner::createRebuildDbButton()
{
   auto btn = new ImageButton(this);
   //btn->setCheckable( false );
   //btn->setChecked( false );

   LiveSkin const & skin = m_app.m_skin;
   int bw = 23;
   int bh = bw;

   QFont font = getPluginDirButtonFont();
   QString msg = QChar(static_cast<ushort>(fa::upload));
   //msg += " AddDir";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   ico = createImageFromText( 1,1, msg, font, QColor(79,254,29), skin.headerBtnFillColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico, 0, 1 );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}
