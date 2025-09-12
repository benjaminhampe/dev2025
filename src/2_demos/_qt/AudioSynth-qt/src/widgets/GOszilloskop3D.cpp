#include "GOszilloskop3D.hpp"
#include "parseQtKey.hpp"
#include <QPainter>
#include <QSurfaceFormat>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#include <de/gpu/GL_debug_layer.h>

using namespace de::audio;

GOszilloskop3D::GOszilloskop3D(QOpenGLContext *sharedContext, QWidget* parent )
   : QOpenGLWidget( parent )
   , m_sharedContext( sharedContext )
   , m_driver( nullptr )
   , m_fpsTimerId( 0 )
   , m_renderFlags( RenderWaveform | RenderSpektrum | RenderTriangles | RenderLines | RenderXLog10 )
   , m_inputSignal( nullptr )
   , m_channelCount( 2 )
   , m_sampleRate( 48000 )
   , m_sizeX( 2000.0f)
   , m_sizeY( 500.0f)
   , m_sizeZ( 20000.0f)
   , m_fft( 8*1024, false )
{
   setAttribute( Qt::WA_OpaquePaintEvent );
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 64, 64 );
   setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   //setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );

   m_cgWaveform.addStop( 0, dbRGBA(0,0,0) );
   m_cgWaveform.addStop( 0.1f, dbRGBA(255,255,255) );
   m_cgWaveform.addStop( 0.2f, dbRGBA(0,0,255) );
   m_cgWaveform.addStop( 0.3f, dbRGBA(0,20,0) );
   m_cgWaveform.addStop( 0.6f, dbRGBA(0,200,0) );
   m_cgWaveform.addStop( 0.8f, dbRGBA(255,255,0) );
   m_cgWaveform.addStop( 1.0f, dbRGBA(255,0,0) );
//   m_cgWaveform.addStop( 1.1f, dbRGBA(255,0,255) );
//   m_cgWaveform.addStop( 1.3f, dbRGBA(0,0,255) );
//   m_cgWaveform.addStop( 1.5f, dbRGBA(0,0,155) );
   m_cgWaveform.addStop( 2.5f, dbRGBA(205,105,5) );

   m_cgSpektrum.addStop( 0, dbRGBA(0,0,0) );
   m_cgSpektrum.addStop( 0.45, dbRGBA(125,125,125) );
   m_cgSpektrum.addStop( 0.5, dbRGBA(0,0,255) );
   m_cgSpektrum.addStop( 0.6, dbRGBA(0,200,0) );
   m_cgSpektrum.addStop( 0.8, dbRGBA(255,255,0) );
   m_cgSpektrum.addStop( 1.0, dbRGBA(255,0,0) );
   //startFpsTimer();
}

GOszilloskop3D::~GOszilloskop3D()
{
   stopFpsTimer();

   if ( m_driver )
   {
      m_driver->close();
      delete m_driver;
      m_driver = nullptr;
   }
}

void
GOszilloskop3D::initializeGL()
{
    context()->setShareContext(m_sharedContext);
    initializeOpenGLFunctions();

    // GT_init();
    //setupDspElement( 1024, 2, 48000 );
    // m_renderer.initializeGL();

    m_driver = de::gpu::createVideoDriver( width(), height(), uint64_t( winId() ) );
    m_driver->setCamera(&m_camera0);
    //m_driver->setCamera( &m_camera );

    startFpsTimer();
}

void
GOszilloskop3D::resizeGL(int w, int h)
{
    if (m_driver)
    {
        m_driver->resize( w, h );
    }

    // glViewport(0, 0, w, h);
    // m_renderer.resizeGL( w, h );

    // //m_modelMat = glm::mat4(1.0f);
    // //m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));
    // //m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
    // g_camera.setScreenSize(w,h);
    // g_camera.update();
}

void
GOszilloskop3D::paintGL()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }
    
    makeCurrent();
    render();

    // glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // g_camera.update();
    // m_renderer.paintGL( &g_camera );
}

/*
void
GOszilloskop3D::paintEvent( QPaintEvent* event )
{
   render();
}

void GOszilloskop3D::resizeEvent( QResizeEvent* event )
{
   if ( !m_driver ) { return; } // No driver.
   int w = event->size().width();
   int h = event->size().height();
   m_driver->resize( w, h );
   //emit_canvasResized();
   // update(); // Seems to create a cycle ( infinite loop )
   //render();
}
*/

void GOszilloskop3D::render()
{
   if ( !m_driver )
   {
       DE_ERROR("No driver")
       return;
   }
   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
   if ( w < 1 || h < 1 )
   {
       DE_ERROR("No canvas")
       return;
   }

   m_driver->setClearColor( glm::vec4(0.01f, 0.01f, 0.01f, 1.f) ); // dark grey
   m_driver->beginRender();

   auto camera = m_driver->getCamera();
   if ( camera )
   {
      camera->setFOV( 87.0f );
      camera->lookAt( glm::vec3(25,704,-318), glm::vec3(25,637.5,-210) );
   }

   de::TRSd trs;

   //// Draw 'L' TriangleMatrix below
   //   trs = de::gpu::TRSd();
   //   trs.setTX( -(45.0f + m_sizeX) );
   //   trs.setTZ( 0.0f );
   //   trs.setRZ( -90.0f );
   //   trs.setRX( 35.0f );
   //   //trs.setSX( -1.0f );
   //   m_driver->setModelMatrix( trs.getModelMatrix() );
   //   m_driver->draw3D( m_trianglesL );

   //// Draw 'R' TriangleMatrix below
   //   trs = de::gpu::TRSd();
   //   trs.setTX( 45.0f + m_sizeX );
   //   trs.setTZ(  0.0f );
   //   trs.setRZ( 90.0f );
   //   trs.setRX( 35.0f );
   //   m_driver->setModelMatrix( trs.getModelMatrix() );
   //   m_driver->draw3D( m_trianglesR );

   // int dBscale = 1;

   if ( m_renderFlags & RenderSpektrum )
   {
      float sampleRate_over_fftSize = float( m_sampleRate / m_fft.size() );
      uint32_t xAxisScaleLog10 = (m_renderFlags & RenderXLog10) != 0;

//      createMatrixTriangleMesh( 0, m_trianglesL, m_shiftMatrixL.m_data,
//         2*m_sizeX, m_sizeY, m_sizeZ, m_cgSpektrum, 1,
//         sampleRate_over_fftSize, xAxisScaleLog10 );

      createMatrixTriangleMesh( 1, m_trianglesR, m_shiftMatrixR.m_data,
         2.0f*m_sizeX, m_sizeY, m_sizeZ, m_cgSpektrum, 1,
         sampleRate_over_fftSize, xAxisScaleLog10 );

        m_trianglesR.upload();

      // Draw 'R' TriangleMatrix below
      trs.reset();
      trs.pos.x = -m_sizeX;
      trs.pos.z = 0.0;
      trs.update();
      m_driver->setModelMatrix( trs.modelMat );
      m_driver->getSMaterialRenderer()->draw3D( m_trianglesR );
   }

   /*
   if ( m_renderFlags & RenderWaveform )
   {
      if ( m_renderFlags & RenderLines )
      {
         int n = m_wavlinesL.m_data.size();
         float fx = 4.0f / float( n );
         float dx = m_wavlinesL.m_sizeX;
         float dy = m_wavlinesL.m_sizeY;
         float dz = m_wavlinesL.m_sizeZ / float( n );
         float tz = 0.0f;
         float sx = 5.0f;
         for ( int i = n-1; i >= 0; i-- )
         {
            de::TRSd trs;
            trs.pos.x = -2.5f*m_sizeX;
            trs.pos.z = dz * i;
            trs.update();
            //trs.setSX( -sx );
            //sx -= fx;
            //sz += 0.075f;
            m_driver->setModelMatrix( trs.modelMat );


            m_driver->getSMaterialRenderer()->draw3D( *m_wavlinesL.m_data[ i ] );
         }

         tz = 0.0f;
         sx = 5.0f;
         for ( int i = n-1; i >= 0; i-- )
         {
            de::TRSd trs;
            trs.pos.x = 2.5f*m_sizeX;
            trs.pos.z = dz * i;
            trs.update();
            //trs.setSX( sx );
            //sx -= fx;
            //sz += 0.075f;
            m_driver->setModelMatrix( trs.modelMat );
            m_driver->getSMaterialRenderer()->draw3D( *m_wavlinesR.m_data[ i ] );
         }
      }

//      float sampleRate_over_fftSize = 0.0f;

//      createMatrixTriangleMesh( 0, m_trianglesL,
//         m_shiftMatrixL.m_data,
//         m_sizeX, m_sizeY, m_sizeZ, m_cgWaveform, 0, sampleRate_over_fftSize, 0 );

//      createMatrixTriangleMesh( 1, m_trianglesR,
//         m_shiftMatrixR.m_data,
//         m_sizeX, m_sizeY, m_sizeZ, m_cgWaveform, 0, sampleRate_over_fftSize, 0 );

//      // Draw 'L' TriangleMatrix below
//      trs = de::gpu::TRSd();
//      trs.setTX( -m_sizeX );
//      trs.setTZ( 0.0f );
//      m_driver->setModelMatrix( trs.getModelMatrix() );
//      m_driver->draw3D( m_trianglesL );

//      // Draw 'R' TriangleMatrix below
//      trs = de::gpu::TRSd();
//      trs.setTX( m_sizeX );
//      trs.setTZ(  0.0f );
//      m_driver->setModelMatrix( trs.getModelMatrix() );
//      m_driver->draw3D( m_trianglesR );

   }
*/


   //m_driver->getSceneManager()->render();

//   m_driver->resetModelMatrix();
//   m_driver->draw2DPerfOverlay5x8();
   m_driver->endRender();
}

void GOszilloskop3D::startFpsTimer()
{
   stopFpsTimer();
   m_fpsTimerId = startTimer( 32, Qt::CoarseTimer );
}

void GOszilloskop3D::stopFpsTimer()
{
   if ( m_fpsTimerId )
   {
      killTimer( m_fpsTimerId );
      m_fpsTimerId = 0;
   }
}

void GOszilloskop3D::clearInputSignals()
{
   m_inputSignal = nullptr;
}

void GOszilloskop3D::setInputSignal( int i, IDspChainElement* input )
{
   m_inputSignal = input;
}

void
GOszilloskop3D::setupDspElement(
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   m_channelCount = dstChannels;
   m_sampleRate = dstRate;

   // Holds all interleaved input,
   // is then cut channelwise into m_channelBuffer
//   if ( m_inputBuffer.size() != dstFrames * dstChannels )
//   {
//      m_inputBuffer.resize( dstFrames * dstChannels );
//   }

   // Holds all frames of one single channel to be processed.
   // Is feed this mono way to shiftmatrices L and R.
   DSP_RESIZE( m_channelBuffer, dstFrames );

   // New fft mode:
   if ( m_renderFlags & RenderSpektrum ) // Only in FFT mode
   {
      uint32_t n = 1024;   // The x size ( colums ) of shift matrices L+R

      if ( n != m_fft.size() ) // we need the shiftMatrix to have colums
      {
         n = m_fft.size(); // exactly of count fftSize for 2^x performance reasons.
      }

      // Input shift buffer in full fft size
      DSP_RESIZE( m_fftL, n );
      DSP_RESIZE( m_fftR, n );

      // temporary buffer for the fft output
      DSP_RESIZE( m_fftOutput, n );

      // Keeps only half of spektrum, half of fftSize
      m_shiftMatrixL.resize( n/8, 64 );
      m_shiftMatrixR.resize( n/8, 128 );
   }
   else
   {
      // Default waveform:
      auto n = std::max( dstFrames, uint32_t(256) );
      m_shiftMatrixL.resize( n, n );
      m_shiftMatrixR.resize( n, n );
   }

//   // New fft mode:
//   if ( m_renderFlags & RenderWaveform ) // Only in FFT mode
//   {
//      // Input shift buffer in full fft size
//      DSP_RESIZE( m_fftL, n );
//      DSP_RESIZE( m_fftR, n );

//      // temporary buffer for the fft output
//      DSP_RESIZE( m_fftOutput, n );

//      // Keeps only half of spektrum, half of fftSize
//      m_shiftMatrixL.resize( n/8, 64 );
//      m_shiftMatrixR.resize( n/8, 128 );
//   }
//   else
//   {
//      // Default waveform:
//      auto n = std::max( dstFrames, uint32_t(256) );
//      m_shiftMatrixL.resize( n, n );
//      m_shiftMatrixR.resize( n, n );
//   }

}


uint64_t
GOszilloskop3D::readDspSamples(
   double pts,
   float* dst,
   uint32_t dstFrames,
   uint32_t dstChannels,
   double dstRate )
{
   uint64_t dstSamples = dstFrames * dstChannels;
   if ( !m_inputSignal )
   {
      DSP_FILLZERO( dst, dstSamples );
      return dstSamples;
   }

   // Can be called once at engine start
   // Does only something if need to.
   setupDspElement( dstFrames, dstChannels, dstRate );

   // We have all buffers prepared lets read the input...
   m_inputSignal->readDspSamples( pts, dst, dstFrames, dstChannels, dstRate );
//   uint64_t retSamples = m_inputSignal->readSamples( pts,
//                         m_inputBuffer.data(), dstFrames, dstChannels, dstRate );
//   if ( retSamples != dstSamples )
//   {
//      DE_WARN("retSamples != dstSamples")
//   }

   if ( !isVisible() ) //  && isBypassed()
   {
      return dstSamples;
   }

//   DSP_FILLZERO( m_inputBuffer );
//   DSP_FILLZERO( m_channelBuffer );

   if ( m_renderFlags & RenderSpektrum )
   {
      // The shiftBuffer has collected enough samples for a new matrix row.
      // Matrix should have half fftSize in x direction == m_colCount
      // Reads and overwrites the front row in shift matrix only.
      // Bad: means we do more ffts than we would doing when rendering

      auto onFull = [&] ()
      {
         m_fft.setInput( m_fftR.data(), m_fft.size() );
         m_fft.fft();
         m_fft.getOutputInDecibel( m_fftOutput.data(), m_fft.size() );
         m_shiftMatrixR.push( m_fftOutput.data(), m_fft.size() / 8 );
      };

      // Fuse stereo to mono = (L+R)/2
      DSP_FUSE_STEREO_TO_MONO( m_channelBuffer.data(), dst, dstFrames, dstChannels );

      // Push fused stereo to mono into the 3D ShiftMatrix...
      m_fftR.push( "m_fftR", m_channelBuffer.data(), dstFrames, false, onFull );
   }

   // Feed waveforms L+R
   if ( m_renderFlags & RenderWaveform )
   {
      // Feed L waveform
      if ( dstChannels > 0 )
      {
         DSP_GET_CHANNEL( m_channelBuffer.data(), dstFrames, dst, dstFrames, 0, dstChannels );
         m_wavlinesL.push( m_channelBuffer.data(), dstFrames );
      }

      // Feed R waveform
      if ( dstChannels > 1 )
      {
         DSP_GET_CHANNEL( m_channelBuffer.data(), dstFrames, dst, dstFrames, 1, dstChannels );
         m_wavlinesR.push( m_channelBuffer.data(), dstFrames );
      }
   }

   //DSP_COPY( m_inputBuffer.data(), dst, dstSamples );
   return dstSamples;
}


void
GOszilloskop3D::hideEvent( QHideEvent* event )
{
   stopFpsTimer();
   //DE_DEBUG("")
   event->accept();
}

void
GOszilloskop3D::showEvent( QShowEvent* event )
{
   //DE_DEBUG("")
   startFpsTimer();
   event->accept();
}

void
GOszilloskop3D::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_fpsTimerId )
    {
        // render();
        update();
    }
}

void
GOszilloskop3D::mouseMoveEvent( QMouseEvent* event )
{
   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::MOUSE_MOVE;
      post.mouseMoveEvent.x = event->x();
      post.mouseMoveEvent.y = event->y();
      //m_driver->onEvent( post );

      /*
      de::SEvent post;
      post.type = de::EventType::MOUSE;
      post.mouseEvent.m_flags = de::MouseEvent::Moved;
      post.mouseEvent.m_x = event->x();
      post.mouseEvent.m_y = event->y();
      post.mouseEvent.m_wheelY = 0.0f;
      m_driver->postEvent( post );
      */
   }
}

void
GOszilloskop3D::mousePressEvent( QMouseEvent* event )
{
   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::MOUSE_PRESS;
      post.mousePressEvent.flags = de::MouseFlag::Pressed;
      post.mousePressEvent.x = event->x();
      post.mousePressEvent.y = event->y();

      if ( event->button() == Qt::LeftButton )
      {
         post.mousePressEvent.buttons |= de::MouseButton::Left;
      }
      else if ( event->button() == Qt::RightButton )
      {
         post.mousePressEvent.buttons |= de::MouseButton::Right;
      }
      else if ( event->button() == Qt::MiddleButton )
      {
         post.mousePressEvent.buttons |= de::MouseButton::Middle;
      }
      //m_driver->postEvent( post );

   }

   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
}

void
GOszilloskop3D::mouseReleaseEvent( QMouseEvent* event )
{
   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::MOUSE_RELEASE;
      post.mouseReleaseEvent.flags = de::MouseFlag::Released;
      post.mouseReleaseEvent.x = event->x();
      post.mouseReleaseEvent.y = event->y();
      if ( event->button() == Qt::LeftButton )
      {
         post.mouseReleaseEvent.buttons |= de::MouseButton::Left;
      }
      else if ( event->button() == Qt::RightButton )
      {
         post.mouseReleaseEvent.buttons |= de::MouseButton::Right;
      }
      else if ( event->button() == Qt::MiddleButton )
      {
         post.mouseReleaseEvent.buttons |= de::MouseButton::Middle;
      }
      //m_driver->postEvent( post );
   }

   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   //m_MouseMove = { 0,0 };
}


void
GOszilloskop3D::wheelEvent( QWheelEvent* event )
{
   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::MOUSE_WHEEL;
      post.mouseWheelEvent.x = event->angleDelta().x();
      post.mouseWheelEvent.y = event->angleDelta().y();
      //m_driver->postEvent( post );
   }

   //event->accept();
}

void
GOszilloskop3D::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")

   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::KEY_PRESS;
      post.keyPressEvent.key = convertQtKey(event->key());
      post.keyPressEvent.modifiers = de::KeyModifier::Pressed;

      if ( event->modifiers() & Qt::ShiftModifier )
      {
         post.keyPressEvent.modifiers |= de::KeyModifier::Shift;
      }
      if ( event->modifiers() & Qt::ControlModifier )
      {
         post.keyPressEvent.modifiers |= de::KeyModifier::Ctrl;
      }
      if ( event->modifiers() & Qt::AltModifier )
      {
         post.keyPressEvent.modifiers |= de::KeyModifier::Alt;
      }
      // m_driver->postEvent( post );
   }

   event->accept();
}

void
GOszilloskop3D::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")

   if ( m_driver )
   {
      de::Event post;
      post.type = de::EventType::KEY_RELEASE;
      post.keyReleaseEvent.key = convertQtKey(event->key());
      post.keyReleaseEvent.modifiers = de::KeyModifier::Released;

      if ( event->modifiers() & Qt::ShiftModifier )
      {
         post.keyReleaseEvent.modifiers = de::KeyModifier::Shift;
      }
      if ( event->modifiers() & Qt::ControlModifier )
      {
         post.keyReleaseEvent.modifiers = de::KeyModifier::Ctrl;
      }
      if ( event->modifiers() & Qt::AltModifier )
      {
         post.keyReleaseEvent.modifiers = de::KeyModifier::Alt;
      }
      //m_driver->postEvent( post );
   }

   event->accept();
}


