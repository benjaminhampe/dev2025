#include "Window.h"
#include "App.h"

Window::Window( App & app )
   : QMainWindow( nullptr )
   , m_app( app )
{
   createMenuBar();
   setContentsMargins( 0,0,0,0 );
}

Window::~Window()
{}

void
Window::createMenuBar()
{
   QMenuBar* menuBar = new QMenuBar( nullptr );

   QMenu* fileMenu = menuBar->addMenu( "&Files" );
   QAction* act_open = fileMenu->addAction( "Open file" );
   QAction* act_exit = fileMenu->addAction( "Exit file" );

   QMenu* viewMenu = menuBar->addMenu( "&View" );
//   QAction* act_show_oszi_3d = viewMenu->addAction( "Show 3d Oszilloskop - F3" );
//   QAction* act_show_master = viewMenu->addAction( "Show Master" );
//   QAction* act_show_mixer = viewMenu->addAction( "Show Mixer - F2" );
//   QAction* act_show_playerAB = viewMenu->addAction( "Show Player A+B" );
//   QAction* act_show_playerCD = viewMenu->addAction( "Show Player C+D" );
//   QAction* act_show_playerEF = viewMenu->addAction( "Show Player E+F" );
//   QAction* act_show_seq16 = viewMenu->addAction( "Show Sequencer16 - F4" );
//   QAction* act_show_synth1 = viewMenu->addAction( "Show DetuneSynth 1 - F5" );

   QMenu* helpMenu = menuBar->addMenu( "&Help" );
   QAction* act_help_about = helpMenu->addAction( "Show &About" );

   this->setMenuBar( menuBar );

   connect( act_open, &QAction::triggered, this, &Window::onMenuFileOpen );
   connect( act_exit, &QAction::triggered, this, &Window::onMenuFileExit );

//   connect( act_show_seq16, &QAction::triggered, this, [&] () {
//         m_seq16->setVisible( !m_seq16->isVisible() );
//      });
//   connect( act_show_synth1, &QAction::triggered, this, [&] () {
//         m_detuneSynth->setVisible( !m_detuneSynth->isVisible() );
//      });

   connect( act_help_about, &QAction::triggered, this, &Window::onShowAboutDialog );

}

void
Window::closeEvent( QCloseEvent* event )
{

   qDebug() << "CloseEvent()";

   QMessageBox msb;
   msb.setText("Live wants to end Application");
   msb.setInformativeText("Would you like to save?");
   msb.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
   msb.setDefaultButton( QMessageBox::Save );
   int ret = msb.exec();
   if (ret == QMessageBox::Save)
   {
      m_app.stopAudioMasterBlocking();
      m_app.save( true );
      m_app.hideEditorWindows();
      event->accept();
   }
   else if (ret == QMessageBox::Discard)
   {
      m_app.stopAudioMasterBlocking();
      m_app.save( false );
      m_app.hideEditorWindows();
      event->accept();
   }
   else if (ret == QMessageBox::Cancel)
   {
      //m_app.stopAudioMasterBlocking();
      //m_app.save( false );
      event->ignore();
   }
   //QMainWindow::closeEvent( event );
}

void
Window::onShowAboutDialog()
{
   //AboutDialog dlg( this );
   //dlg.exec();
}

void
Window::onMenuFileExit()
{
   close();
}

void
Window::onMenuFileOpen()
{
   // QApplication::applicationDirPath()
   QStringList files = QFileDialog::getOpenFileNames( this, "Open 3d model file", "G:/media/models" );
   if ( files.count() < 1 )
   {
      qDebug() << "No files selected";
      return;
   }

   if ( files.count() < 1 )
   {
      qDebug() << "No File selected";
      return;
   }

   qDebug() << "Selected File[0] " << files.at( 0 );

//   de::gpu::IVideoDriver * m_Driver = m_Canvas->getVideoDriver();
//   if ( !m_Driver )
//   {
//      qDebug() << "No video driver";
//      return;
//   }

//   QString uri = files.at( 0 );

//   auto sri = uri.toStdString();
//   auto mesh = new de::gpu::SMesh( sri );
//   bool ok = de::gpu::SMeshIO::load( *mesh, sri, m_Driver );
//   if ( ok )
//   {
//      //glm::dvec3 bb = m_Canvas->m_Mesh.getBoundingBox().getSize();
//      //glm::dvec3 scale = glm::dvec3( 100,100,100 ) / bb;
//      //m_Canvas->m_MeshTRS.setS( scale.x, scale.y, scale.z );
//      DE_DEBUG("+ Loaded 3D file ",sri)
//   }
//   else
//   {
//      DE_ERROR("--- Cant Load 3D file ",sri)
//   }

//   m_SceneEditor->addMesh( mesh );
}


