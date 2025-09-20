#include "MainWindow.hpp"
#include <QImageWidget.hpp>

MainWindow::MainWindow()
   : QMainWindow( nullptr )
{
   createMenuBar();
   setContentsMargins( 5,5,5,5 );

//   auto rackWidget = new QImageWidget( this );
//   rackWidget->setLayout( rackLayout );

//   auto splitV = new QSplitter( Qt::Vertical, this );
//   splitV->addWidget( m_waveform3D );
//   splitV->addWidget( rackWidget );
//   splitV->setChildrenCollapsible( true );
////   splitV->setStretchFactor( 0, 1 );
////   splitV->setStretchFactor( 1, 0 );

   auto centralLayout = new QVBoxLayout();
   centralLayout->setContentsMargins( 0,0,0,0 );
   centralLayout->setSpacing( 0 );
//   centralLayout->addWidget( splitV );
//   centralLayout->addStretch( splitV );

   auto centralWidget = new QImageWidget( this );
   centralWidget->setLayout( centralLayout );
   setCentralWidget( centralWidget );

   //m_synth2Delay->setInputSignal( m_synth2 );
   //m_synth2Delay->setBypassed( true );
//   m_synth2LFO->setInputSignal( m_synth2Delay );
//   m_synth2LFO->setBypassed( true );
   //m_synth2Waveform->setInputSignal( m_synth2Delay );
   //m_synth2Volume->setInputSignal( m_synth2Waveform );
   //m_synth2LevelMeter->setInputSignal( m_synth2Volume );

}

MainWindow::~MainWindow()
{}

void
MainWindow::createMenuBar()
{
   QMenuBar* menuBar = new QMenuBar( nullptr );

   QMenu* fileMenu = menuBar->addMenu( "&Files" );
   QAction* act_open = fileMenu->addAction( "Open file" );
   QAction* act_exit = fileMenu->addAction( "Exit file" );

   QMenu* viewMenu = menuBar->addMenu( "&View" );
   QAction* act_show_oszi_3d = viewMenu->addAction( "Show 3d Oszilloskop - F3" );
   act_show_oszi_3d->setCheckable( true );

   QMenu* helpMenu = menuBar->addMenu( "&Help" );
   QAction* act_help_about = helpMenu->addAction( "Show &About" );

   this->setMenuBar( menuBar );

   connect( act_open, &QAction::triggered, this,
            [&] (bool checked) { } );

   connect( act_exit, &QAction::triggered, this,
            [&] (bool checked) { close(); } );

   connect( act_help_about, &QAction::triggered, this,
            [&] (bool checked) {/*  AboutDialog dlg( this ); dlg.exec(); */ } );

//   connect( act_show_oszi_3d, &QAction::triggered, this,
//            [&] (bool checked) { m_waveform3D->setVisible( checked ); } );

//   connect( act_show_seq16, &QAction::triggered, this, [&] () {
//         m_seq16->setVisible( !m_seq16->isVisible() );
//      });
}

void
MainWindow::onMenuFileOpen()
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


