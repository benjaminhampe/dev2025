#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include "ShiftMatrix.hpp"
#include <de/audio/dsp/IDspChainElement.hpp>
#include <functional>
#include <de_kissfft.hpp>

//#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <QOffscreenSurface>

#include <QTimer>
#include <QDebug>

#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFunctions_4_5_Core>

#include <QtGlobal>
#include <QTimerEvent>
#include <QMouseEvent>

#if QT_VERSION_MAJOR == 5
#elif QT_VERSION_MAJOR == 6
    #include <QOpenGLVersionFunctionsFactory>
#else
#endif

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

// =======================================================================
struct GOszilloskop3D : public QOpenGLWidget // public QWidget
                      , protected QOpenGLFunctions_4_5_Core
                      , public de::audio::IDspChainElement
// =======================================================================
{
   Q_OBJECT
public:
   enum eRenderFlag
   {
      RenderNone = 0,
      RenderLines = 0x01,
      RenderTriangles = 0x02,
      RenderWaveform = 0x04,
      RenderSpektrum = 0x08,
      RenderXLog10 = 0x10,
      RenderFast = 0x20,
      eRenderFlagCount = 5
   };
   GOszilloskop3D(QOpenGLContext *sharedContext, QWidget* parent = nullptr);
   ~GOszilloskop3D() override;

   uint64_t
   readDspSamples( double pts, float* dst,
      uint32_t frames, uint32_t channels, double rate ) override;

signals:
public slots:
   void
   setupDspElement(
      uint32_t frames, uint32_t channels, double rate ) override;

   void
   clearInputSignals() override;

   void
   setInputSignal( int i, de::audio::IDspChainElement* input ) override;

private slots:
   void startFpsTimer();
   void stopFpsTimer();
protected:
   void initializeGL() override;
   void resizeGL(int w, int h) override;
   void paintGL() override;

   void render();
   void timerEvent( QTimerEvent* event) override;
   //void paintEvent( QPaintEvent* event ) override;
   //void resizeEvent( QResizeEvent* event ) override;
   void hideEvent( QHideEvent* event ) override;
   void showEvent( QShowEvent* event ) override;
   void keyPressEvent( QKeyEvent* event ) override;
   void keyReleaseEvent( QKeyEvent* event ) override;
   void mousePressEvent( QMouseEvent* event ) override;
   void mouseReleaseEvent( QMouseEvent* event ) override;
   void mouseMoveEvent( QMouseEvent* event ) override;
   void wheelEvent( QWheelEvent* event ) override;
protected:
   DE_CREATE_LOGGER("GOszilloskop3D")
   QOpenGLContext* m_sharedContext;
   de::gpu::VideoDriver* m_driver;
   de::gpu::Camera m_camera0;
   int m_fpsTimerId;
   uint32_t m_renderFlags;
   de::audio::IDspChainElement* m_inputSignal;
   uint32_t m_channelCount;
   uint32_t m_sampleRate;  // for fft
   uint32_t m_frameIndex;
   float L_min;
   float L_max;
   float R_min;
   float R_max;
   float m_sizeX;
   float m_sizeY;
   float m_sizeZ;
   std::vector< float > m_inputBuffer;
   std::vector< float > m_channelBuffer;
   //std::vector< float > m_inputBuffer;
   //std::vector< float > m_outputBuffer; // in deciBel dB
   ShiftMesh_LineStrips m_wavlinesL;
   ShiftMesh_LineStrips m_wavlinesR;
   de::gpu::SMeshBuffer m_trianglesL;
   de::gpu::SMeshBuffer m_trianglesR;

   ShiftMatrix< float > m_shiftMatrixL;
   ShiftMatrix< float > m_shiftMatrixR;
   de::audio::ShiftBuffer m_fftL;   // input collector
   de::audio::ShiftBuffer m_fftR;   // output collector
   std::vector< float > m_fftOutput;         // intermediary output, is fed to shiftMatrix.
   de::KissFFT m_fft;
//   de::gpu::SMeshBuffer m_wavtrisFrontL;
//   de::gpu::SMeshBuffer m_wavtrisFrontR;
   de::LinearColorGradient m_cgWaveform;
   de::LinearColorGradient m_cgSpektrum;
};

