#include "MainWindow.hpp"

MainWindow::MainWindow() : QMainWindow( nullptr )
{
   setContentsMargins( 5,5,5,5 );
   createMenuBar();

   // Create a custom OpenGL context
   QSurfaceFormat format;
   format.setVersion(4, 5);
   format.setProfile(QSurfaceFormat::CoreProfile);
   format.setOption(QSurfaceFormat::DebugContext);
   format.setDepthBufferSize(24);
   format.setStencilBufferSize(8);
   format.setSamples(0);
   format.setAlphaBufferSize(0);
   format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
   //format.setColorSpace(QSurfaceFormat::sRGBColorSpace);
   format.setStereo(false);
   format.setSwapInterval(1);
   QSurfaceFormat::setDefaultFormat(format);

   m_customContext = new QOpenGLContext;
   m_customContext->setFormat(format);
   if (!m_customContext->create())
   {
       qDebug() << "Failed to create custom OpenGL context";
       //return -1;
   }

   // Create a temporary offscreen surface to make the custom context current
   m_offscreenSurface = new QOffscreenSurface;
   m_offscreenSurface->setFormat(m_customContext->format());
   m_offscreenSurface->create();

   m_customContext->makeCurrent(m_offscreenSurface);


   m_audioEngine = new GEngine( this );
   m_waveform3D = new GOszilloskop3D( m_customContext );
   m_audioRecorder = new GAudioRecorder( this );
   m_mixer = new GMixer( this );
   m_master = new GMasterDeck( this );
   m_deckAB = new GFilePlayerDeck( this );
   m_deckCD = new GFilePlayerDeck( this );
   m_deckEF = new GFilePlayerDeck( this );
   m_detuneSynth = new GDetuneSynthDeck( this );
   m_betterSynth = new GBetterSynthDeck( this );
   m_vstDeck = new GVstDeck( this );
   m_sequencer = new GSequencerCtl( this );
   m_piano2D = new GMidiMasterKeyboard( this );

   //m_Piano3D = new Piano3D( this );
   auto rackLayout = new QVBoxLayout();
   rackLayout->setContentsMargins( 0,0,0,0 );
   rackLayout->setSpacing( 0 );
   rackLayout->addWidget( m_audioEngine );
   rackLayout->addWidget( m_audioRecorder );
   rackLayout->addWidget( m_mixer );
   rackLayout->addWidget( m_master );
   rackLayout->addWidget( m_deckAB );
   rackLayout->addWidget( m_deckCD );
   rackLayout->addWidget( m_deckEF );
   rackLayout->addWidget( m_detuneSynth );
   rackLayout->addWidget( m_betterSynth );
   rackLayout->addWidget( m_vstDeck );
   rackLayout->addWidget( m_sequencer,1 );
   rackLayout->addWidget( m_piano2D );
   auto rackWidget = new GRack( this );
   rackWidget->setLayout( rackLayout );

   auto body = new Body( this );
   body->addWidget( m_waveform3D );
   body->addWidget( rackWidget );
   body->setChildrenCollapsible( true );
   setCentralWidget( body );

   m_mixer->dsp()->setInputSignal( 0, m_detuneSynth );
   m_mixer->dsp()->setInputSignal( 1, m_deckAB );
   m_mixer->dsp()->setInputSignal( 2, m_deckCD );
   m_mixer->dsp()->setInputSignal( 3, m_deckEF );
   m_mixer->dsp()->setInputSignal( 4, m_betterSynth );
   m_mixer->dsp()->setInputSignal( 5, m_vstDeck );

   m_master->setInputSignal( 0, m_mixer->dsp() );
   //m_master->m_timeStretch->setInputSignal( m_mixer );

   m_waveform3D->setInputSignal( 0, m_master );

   m_audioEngine->dsp()->setInputSignal( 0, m_waveform3D );
//   m_master->m_delay->dsp()->setInputSignal( 0, m_mixer );
//   m_master->m_pan->dsp()->setInputSignal( 0, m_master->m_delay->dsp() );
//   m_master->m_lowPass3->dsp()->setInputSignal( 0, m_master->m_pan->dsp() );
//   m_master->m_overdrive->setInputSignal( 0, m_master->m_lowPass3->dsp() );
//   m_master->m_lowPass4->dsp()->setInputSignal( 0, m_master->m_overdrive );
//   m_master->m_bassBoost->dsp()->setInputSignal( 0, m_master->m_lowPass4->dsp() );
//   m_master->m_compressor->dsp()->setInputSignal( 0, m_master->m_bassBoost->dsp() );
//   m_master->m_limiter->dsp()->setInputSignal( 0, m_master->m_compressor->dsp() );
//   m_master->m_waveform->setInputSignal( 0, m_master->m_limiter->dsp() );
//   m_waveform3D->setInputSignal( 0, m_master->m_waveform );
//   m_master->m_spektrum->setInputSignal( m_waveform3D );
//   m_master->m_volume->setInputSignal( 0, m_master->m_spektrum );
//   m_master->m_levelMeter->dsp()->setInputSignal( 0, m_master->m_volume );
//   m_audioRecorder->setInputSignal( 0, m_master->m_levelMeter->dsp() );
//   m_master->m_menu->setInputSignal( 0, m_audioRecorder );
   //m_waveform->setInputSignal( 0,
   // m_inputSignal );

   m_synths.push_back( m_detuneSynth );
   m_synths.push_back( m_betterSynth );
   m_synths.push_back( m_vstDeck );

   m_piano2D->setSynth( 0, m_detuneSynth );
   m_piano2D->setSynth( 1, m_betterSynth );
   m_piano2D->setSynth( 2, m_vstDeck );



   //m_midiFilePlayer->setSynth( 0, m_detuneSynth );
   //m_midiSequencer->setSynth( 0, m_betterSynth );
   //m_sequencer->setSynth( 0, m_betterSynth );
   //m_Piano3D->setSynth( 0, m_synth );
   //m_Piano3D->setSynth( 1, m_synth2 );
   //m_Piano3D->setFps( 20 );

   m_audioRecorder->setVisible( false );

   m_master->setBypassed( false  );
   m_master->setMinified( false );

   m_deckAB->setBypassed( true );
   m_deckAB->setMinified( false );
   m_deckAB->setVisible( false );

   m_deckCD->setBypassed( true );
   m_deckCD->setMinified( false );
   m_deckCD->setVisible( false );

   m_deckEF->setBypassed( true );
   m_deckEF->setMinified( false );
   m_deckEF->setVisible( false );

   m_detuneSynth->setBypassed( true );
   m_detuneSynth->setMinified( false );
   m_detuneSynth->setVisible( false );

   m_betterSynth->setBypassed( true );
   m_betterSynth->setMinified( false );
   m_betterSynth->setVisible( false );

   m_vstDeck->setBypassed( false );
   m_vstDeck->setMinified( false );
   m_vstDeck->setVisible( false );

   m_sequencer->setVisible( false );

   //updateViewMenu();

   //m_audioEngine.play();
}

MainWindow::~MainWindow()
{}

void
MainWindow::updateViewMenu()
{
   m_act_show_oszi_3d->blockSignals( true );
   m_act_show_oszi_3d->setChecked( m_waveform3D->isVisible() );
   m_act_show_oszi_3d->blockSignals( false );

   m_act_show_recorder->blockSignals( true );
   m_act_show_recorder->setChecked( m_audioRecorder->isVisible() );
   m_act_show_recorder->blockSignals( false );

   m_act_show_master->blockSignals( true );
   m_act_show_master->setChecked( m_master->isVisible() );
   m_act_show_master->blockSignals( false );

   m_act_show_mixer->blockSignals( true );
   m_act_show_mixer->setChecked( m_mixer->isVisible() );
   m_act_show_mixer->blockSignals( false );

   m_act_show_playerAB->blockSignals( true );
   m_act_show_playerAB->setChecked(m_deckAB->isVisible() );
   m_act_show_playerAB->blockSignals( false );

   m_act_show_playerCD->blockSignals( true );
   m_act_show_playerCD->setChecked( m_deckCD->isVisible() );
   m_act_show_playerCD->blockSignals( false );

   m_act_show_playerEF->blockSignals( true );
   m_act_show_playerEF->setChecked( m_deckEF->isVisible() );
   m_act_show_playerEF->blockSignals( false );

   m_act_show_seq16->blockSignals( true );
   m_act_show_seq16->setChecked( m_sequencer->isVisible() );
   m_act_show_seq16->blockSignals( false );

   m_act_show_detunesynth->blockSignals( true );
   m_act_show_detunesynth->setChecked( m_detuneSynth->isVisible() );
   m_act_show_detunesynth->blockSignals( false );

   m_act_show_bettersynth->blockSignals( true );
   m_act_show_bettersynth->setChecked( m_betterSynth->isVisible() );
   m_act_show_bettersynth->blockSignals( false );

   m_act_show_clipeditor->blockSignals( true );
   m_act_show_clipeditor->setChecked( m_sequencer->isVisible() );
   m_act_show_clipeditor->blockSignals( false );

   m_act_show_vsthost->blockSignals( true );
   m_act_show_vsthost->setChecked( m_vstDeck->isVisible() );
   m_act_show_vsthost->blockSignals( false );

   m_act_show_piano2d->blockSignals( true );
   m_act_show_piano2d->setChecked( m_piano2D->isVisible() );
   m_act_show_piano2d->blockSignals( false );
}

void
MainWindow::createMenuBar()
{
   m_menuBar = new QMenuBar( nullptr );

   m_fileMenu = m_menuBar->addMenu( "&Files" );
   m_act_open = m_fileMenu->addAction( "Open file" );
   m_act_exit = m_fileMenu->addAction( "Exit file" );

   m_viewMenu = m_menuBar->addMenu( "&View" );
   m_act_show_oszi_3d = m_viewMenu->addAction( "Show 3d Oszilloskop - F3" );
   m_act_show_recorder = m_viewMenu->addAction( "Show Recorder" );
   m_act_show_master = m_viewMenu->addAction( "Show Master" );
   m_act_show_mixer = m_viewMenu->addAction( "Show Mixer - F2" );
   m_act_show_playerAB = m_viewMenu->addAction( "Show Player A+B" );
   m_act_show_playerCD = m_viewMenu->addAction( "Show Player C+D" );
   m_act_show_playerEF = m_viewMenu->addAction( "Show Player E+F" );
   m_act_show_seq16 = m_viewMenu->addAction( "Show Sequencer16 - F4" );
   m_act_show_detunesynth = m_viewMenu->addAction( "Show DetuneSynth - F5" );
   m_act_show_bettersynth = m_viewMenu->addAction( "Show BetterSynth - F6" );
   m_act_show_clipeditor = m_viewMenu->addAction( "Show MidiClipEditor - F7" );
   m_act_show_vsthost = m_viewMenu->addAction( "Show VST x64 Plugin-Host - F8" );
   m_act_show_piano2d = m_viewMenu->addAction( "Show Piano 2D - F8" );

   m_act_show_oszi_3d->setCheckable( true );
   m_act_show_recorder->setCheckable( true );
   m_act_show_master->setCheckable( true );
   m_act_show_mixer->setCheckable( true );
   m_act_show_playerAB->setCheckable( true );
   m_act_show_playerCD->setCheckable( true );
   m_act_show_playerEF->setCheckable( true );
   m_act_show_seq16->setCheckable( true );
   m_act_show_detunesynth->setCheckable( true );
   m_act_show_bettersynth->setCheckable( true );
   m_act_show_clipeditor->setCheckable( true );
   m_act_show_vsthost->setCheckable( true );
   m_act_show_piano2d->setCheckable( true );

   m_helpMenu = m_menuBar->addMenu( "&Help" );
   m_act_help_about = m_helpMenu->addAction( "Show &About" );

   m_act_show_oszi_3d->setChecked( true );
   m_act_show_recorder->setChecked( false );
   m_act_show_master->setChecked( true );
   m_act_show_mixer->setChecked( false );
   m_act_show_playerAB->setChecked( false );
   m_act_show_playerCD->setChecked( false );
   m_act_show_playerEF->setChecked( false );
   m_act_show_seq16->setChecked( false );
   m_act_show_detunesynth->setChecked( false );
   m_act_show_bettersynth->setChecked( false );
   m_act_show_clipeditor->setChecked( false );
   m_act_show_vsthost->setChecked( false );
   m_act_show_piano2d->setChecked( true );

   this->setMenuBar( m_menuBar );

   connect( m_act_open, &QAction::triggered, this, &MainWindow::onMenuFileOpen );
   connect( m_act_exit, &QAction::triggered, this, &MainWindow::onMenuFileExit );

   connect( m_act_show_oszi_3d, &QAction::triggered, this,
            [&] (bool checked)
            {
               m_waveform3D->blockSignals( true );
               m_waveform3D->setVisible( checked );
               m_waveform3D->blockSignals( false );
            } );
   connect( m_act_show_recorder, &QAction::triggered, this,
            [&] (bool checked)
            {
               m_audioRecorder->setVisible( checked );
            } );
   connect( m_act_show_master, &QAction::triggered, this,
            [&] (bool checked)
            {
               m_master->setVisible( checked );
            } );
   connect( m_act_show_mixer, &QAction::triggered, this,
            [&] (bool checked) { m_mixer->setVisible( checked ); } );
   connect( m_act_show_playerAB, &QAction::triggered, this,
            [&] (bool checked) { m_deckAB->setVisible( checked ); } );
   connect( m_act_show_playerCD, &QAction::triggered, this,
            [&] (bool checked) { m_deckCD->setVisible( checked ); } );
   connect( m_act_show_playerEF, &QAction::triggered, this,
            [&] (bool checked) { m_deckEF->setVisible( checked ); } );/*
   connect( m_act_show_seq16, &QAction::triggered, this,
            [&] (bool checked) { m_seq16->setVisible( checked ); } );*/
   connect( m_act_show_detunesynth, &QAction::triggered, this,
            [&] (bool checked) { m_detuneSynth->setVisible( checked ); } );
   connect( m_act_show_bettersynth, &QAction::triggered, this,
            [&] (bool checked) { m_betterSynth->setVisible( checked ); } );
   connect( m_act_show_clipeditor, &QAction::triggered, this,
            [&] (bool checked) { m_sequencer->setVisible( checked ); } );
   connect( m_act_show_vsthost, &QAction::triggered, this,
            [&] (bool checked) { m_vstDeck->setVisible( checked ); } );
   connect( m_act_show_piano2d, &QAction::triggered, this,
            [&] (bool checked) { m_piano2D->setVisible( checked ); } );
   connect( m_act_help_about, &QAction::triggered,
            this, &MainWindow::onShowAboutDialog );

   //updateViewMenu();
}

void
MainWindow::onShowAboutDialog()
{
   //AboutDialog dlg( this );
   //dlg.exec();
}

void
MainWindow::onMenuFileExit()
{
   close();
}

void
MainWindow::onMenuFileOpen()
{
   // QApplication::applicationDirPath()
   QStringList files =
   QFileDialog::getOpenFileNames( this, "Open 3d model file", "G:/media/models" );

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

void
MainWindow::noteOn( int midiNote, int velocity )
{
   for ( auto synth : m_synths )
   {
      if ( synth )
      {
         synth->sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
      }
   }
}

void
MainWindow::noteOff( int midiNote, int velocity )
{
   for ( auto synth : m_synths )
   {
      if ( synth )
      {
         de::audio::Note note( 0, midiNote, 0, 0 );
         //note.m_noteOff = true;
         synth->sendNote( note );
      }
   }
}

void
MainWindow::keyPressEvent( QKeyEvent* event )
{
   auto key = event->key();
   DE_ERROR("KeyPress(",key,")")

   // DE_DEBUG("KeyPress(",key,")")

   int k = 59; // 59;
   if ( key == Qt::Key_1 ) { noteOn( 59 ); }
   if ( key == Qt::Key_Q ) { noteOn( 60 ); } // C
   if ( key == Qt::Key_2 ) { noteOn( 61 ); }
   if ( key == Qt::Key_W ) { noteOn( 62 ); }
   if ( key == Qt::Key_3 ) { noteOn( 63 ); }
   if ( key == Qt::Key_E ) { noteOn( 64 ); }
   if ( key == Qt::Key_R ) { noteOn( 65 ); }
   if ( key == Qt::Key_5 ) { noteOn( 66 ); }
   if ( key == Qt::Key_T ) { noteOn( 67 ); }
   if ( key == Qt::Key_6 ) { noteOn( 68 ); }
   if ( key == Qt::Key_Z ) { noteOn( 69 ); }
   if ( key == Qt::Key_7 ) { noteOn( 70 ); }
   if ( key == Qt::Key_U ) { noteOn( 71 ); }
   if ( key == Qt::Key_I ) { noteOn( 72 ); } // C
   if ( key == Qt::Key_9 ) { noteOn( 73 ); }
   if ( key == Qt::Key_O ) { noteOn( 74 ); }
   if ( key == Qt::Key_0 ) { noteOn( 75 ); }
   if ( key == Qt::Key_P ) { noteOn( 76 ); }
   if ( key == 220 /*Qt::Key_Udiaeresis*/ ) { noteOn( 77 ); }
   if ( key == 223 /*Qt::Key_SZ*/ ) { noteOn( 78 ); }
   if ( key == 43 /*Qt::Key_Plus */ ) { noteOn( 79 ); }

   k = 48;
   if ( key == Qt::Key_Greater || key == Qt::Key_Less ) { noteOn( k ); } k++; // C
   if ( key == Qt::Key_A ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Y ) { noteOn( k ); } k++;
   if ( key == Qt::Key_S ) { noteOn( k ); } k++;
   if ( key == Qt::Key_X ) { noteOn( k ); } k++;
   //if ( key == Qt::Key_D ) { noteOn( k ); } k++;
   if ( key == Qt::Key_C ) { noteOn( k ); } k++;
   if ( key == Qt::Key_F ) { noteOn( k ); } k++;
   if ( key == Qt::Key_V ) { noteOn( k ); } k++;
   if ( key == Qt::Key_G ) { noteOn( k ); } k++;
   if ( key == Qt::Key_B ) { noteOn( k ); } k++;
   if ( key == Qt::Key_H ) { noteOn( k ); } k++;
   if ( key == Qt::Key_N ) { noteOn( k ); } k++;
   if ( key == Qt::Key_M ) { noteOn( k ); } k++; // C
   if ( key == Qt::Key_K ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Comma ) { noteOn( k ); } k++;
   if ( key == Qt::Key_L ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Period ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Minus ) { noteOn( k ); } k++; // F
   if ( key == Qt::Key_Odiaeresis ) { noteOn( k ); } k++; // Fis
   if ( key == Qt::Key_Adiaeresis ) { noteOn( k ); } k++; // Ä
   if ( key == Qt::Key_NumberSign ) { noteOn( k ); } k++; // #

   //event->accept();

   QMainWindow::keyPressEvent( event );
}

void
MainWindow::keyReleaseEvent( QKeyEvent* event )
{
   auto key = event->key();

   // DE_DEBUG("keyRelease(",key,")")

   int k = 59; // 59;
   if ( key == Qt::Key_1 ) { noteOff( 59 ); }
   if ( key == Qt::Key_Q ) { noteOff( 60 ); }
   if ( key == Qt::Key_2 ) { noteOff( 61 ); }
   if ( key == Qt::Key_W ) { noteOff( 62 ); }
   if ( key == Qt::Key_3 ) { noteOff( 63 ); }
   if ( key == Qt::Key_E ) { noteOff( 64 ); }
   if ( key == Qt::Key_R ) { noteOff( 65 ); }
   if ( key == Qt::Key_5 ) { noteOff( 66 ); }
   if ( key == Qt::Key_T ) { noteOff( 67 ); }
   if ( key == Qt::Key_6 ) { noteOff( 68 ); }
   if ( key == Qt::Key_Z ) { noteOff( 69 ); }
   if ( key == Qt::Key_7 ) { noteOff( 70 ); }
   if ( key == Qt::Key_U ) { noteOff( 71 ); }
   if ( key == Qt::Key_I ) { noteOff( 72 ); }
   if ( key == Qt::Key_9 ) { noteOff( 73 ); }
   if ( key == Qt::Key_O ) { noteOff( 74 ); }
   if ( key == Qt::Key_0 ) { noteOff( 75 ); }
   if ( key == Qt::Key_P ) { noteOff( 76 ); }
   if ( key == 220 /*Qt::Key_Udiaeresis*/ ) { noteOn( 77 ); }
   if ( key == 223 /*Qt::Key_SZ*/ ) { noteOn( 78 ); }
   if ( key == 43 /*Qt::Key_Plus */ ) { noteOn( 79 ); }

   k = 48;
   if ( key == Qt::Key_Greater || key == Qt::Key_Less ) { noteOff( k ); } k++;
   if ( key == Qt::Key_A ) { noteOff( k ); } k++;
   if ( key == Qt::Key_Y ) { noteOff( k ); } k++;
   if ( key == Qt::Key_S ) { noteOff( k ); } k++;
   if ( key == Qt::Key_X ) { noteOff( k ); } k++;
   //if ( key == Qt::Key_D ) { noteOff( k ); } k++;
   if ( key == Qt::Key_C ) { noteOff( k ); } k++;
   if ( key == Qt::Key_F ) { noteOff( k ); } k++;
   if ( key == Qt::Key_V ) { noteOff( k ); } k++;
   if ( key == Qt::Key_G ) { noteOff( k ); } k++;
   if ( key == Qt::Key_B ) { noteOff( k ); } k++;
   if ( key == Qt::Key_H ) { noteOff( k ); } k++;
   if ( key == Qt::Key_N ) { noteOff( k ); } k++;
   if ( key == Qt::Key_M ) { noteOff( k ); } k++;
   if ( key == Qt::Key_K ) { noteOff( k ); } k++;
   if ( key == Qt::Key_Comma ) { noteOff( k ); } k++;
   if ( key == Qt::Key_L ) { noteOff( k ); } k++;
   if ( key == Qt::Key_Period ) { noteOff( k ); } k++;
   if ( key == Qt::Key_Minus ) { noteOff( k ); } k++;
   if ( key == Qt::Key_Odiaeresis ) { noteOff( k ); } k++; // Ö
   if ( key == Qt::Key_Adiaeresis ) { noteOff( k ); } k++; // Ä
   if ( key == Qt::Key_NumberSign ) { noteOff( k ); } k++; // #

   //event->accept();
   QMainWindow::keyReleaseEvent( event );
}
