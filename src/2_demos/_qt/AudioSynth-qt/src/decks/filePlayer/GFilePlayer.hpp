#pragma once
#include <QWidget>
#include <QImage>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QThread>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QFileDialog>

#include <QImageWidget.hpp>
#include <QFont5x8.hpp>
#include <GVolume.hpp>
#include <GLevelMeter.hpp>
#include <GLowPass3.hpp>
#include <GSoundTouch.hpp>
//#include <GCrossFader.hpp>
//#include <GOszilloskop.hpp>
//#include <GSpektrometer.hpp>

#include <de/audio/dsp/IDspChainElement.hpp>
#include <de/audio/buffer/BufferIO.hpp>
#include "GTimeSeek.hpp"


inline QImage
toQImage( de::Image const & img )
{
   uint32_t w = img.w();
   uint32_t h = img.h();

   if ( w < 1 || h < 1 )
   {
      return QImage();
   }

   QImage dst( w,h, QImage::Format_ARGB32 );

   for ( size_t j = 0; j < h; ++j )
   {
      for ( size_t i = 0; i < w; ++i )
      {
         uint32_t color = img.getPixel( i,j );
         int r = dbRGBA_R( color );
         int g = dbRGBA_G( color );
         int b = dbRGBA_B( color );
         int a = dbRGBA_A( color );
         dst.setPixelColor( i,j, QColor( r,g,b,255 ) );
      }
   }

   return dst;
}


namespace de {
namespace audio {

// ============================================================================
class BufferPlayer : public de::audio::IDspChainElement
// ============================================================================
{
public:
   BufferPlayer();
   ~BufferPlayer();

   void rewind()
   {
      m_frameIndex = 0;
   }

   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override;

   virtual void
   setSampleBuffer( Buffer* sampleBuffer )
   {
      m_sampleBuffer = sampleBuffer;
      if ( m_sampleBuffer )
      {
         m_frameCount = m_sampleBuffer->getFrameCount();
         m_sampleRate = m_sampleBuffer->getSampleRate();
      }
      else
      {
         m_frameCount = 0;
         m_sampleRate = 0;
      }
      m_frameIndex = 0;
   }

   void
   setFrameIndex( uint64_t frameIndex )
   {
      m_frameIndex = std::clamp( frameIndex, 0ull, m_frameCount );
   }

   uint32_t getSampleRate() const
   {
      return m_sampleRate;
   }

   uint64_t getFrameIndex() const
   {
      return m_frameIndex;
   }

   uint64_t getFrameCount() const
   {
      return m_frameCount;
   }

   double getDuration() const
   {
      return m_sampleRate * m_frameCount;
   }

   double getPosition() const
   {
      return m_sampleRate * m_frameIndex * 100.0;
   }

   void
   setPosition( double percent )
   {
      uint64_t frameIndex = percent / 100.0 * m_frameCount;
      setFrameIndex( frameIndex );
   }

private:
   Buffer* m_sampleBuffer;

   std::string m_uri;

   //uint32_t m_state;
   uint32_t m_sampleRate;
   uint64_t m_frameCount;
   uint64_t m_frameIndex;

   bool m_isLooping;
   int m_loopIndex;
   int m_loopCount;

/*
   enum eState
   {
      Unloaded = 0,
      Stopped,
      Playing,
      Paused,
      Looping,
      eStateCount
   };

   uint32_t getState() const;

   void load();
   void play();
   void stop();
   void pause();
   */

   // void clearInputSignals();
   // void setInputSignal( de::audio::IDspChainElement* input );

};

} // end namespace audio.
} // end namespace de.


// ============================================================================
class GFilePlayer : public QWidget, public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
   DE_CREATE_LOGGER("GFilePlayer")
public:
   GFilePlayer( bool bRight, QWidget* parent = 0 );
   ~GFilePlayer();

   uint64_t
   readDspSamples(
      double pts,
      float* dst,
      uint32_t dstFrames,
      uint32_t dstChannels,
      double dstRate ) override;

   virtual uint32_t
   getInputSampleRate() const { return m_sampleBuffer.getSampleRate(); }

   enum eState
   {
      Unloaded = 0,
      Stopped,
      Playing,
      Paused,
      Looping,
      eStateCount
   };

   uint32_t getState() const;

   QString getStateStr() const;

   double getDuration() const;

   double getPosition() const;
signals:
public slots:
   void load();
   //void unload();
   void play();
   void stop();
   void pause();
   //void setPos();

void startUpdateTimer();
void stopUpdateTimer();

private slots:
   void on_clicked_load(bool checked);
   void on_clicked_play(bool checked);
   void on_clicked_stop(bool checked);
   void on_slider_pos( int );
   void on_time_seek( float percent );

   // void stopUpdateTimer();
   // void startUpdateTimer( int ms = 67 );
   // void clearInputSignals();
   // void setInputSignal( de::audio::IDspChainElement* input );

protected:
   void timerEvent( QTimerEvent* event ) override;
   void updateLabels();
   void updateSlider();
   // void timerEvent( QTimerEvent* event) override;
   // void paintEvent( QPaintEvent* event ) override;

private:

   QWidget* m_fileInfo;

   QLineEdit* m_edtUri;
   QLineEdit* m_edtState;

   QImageWidget* m_coverArt;

   QPushButton* m_btnLoad;
   QPushButton* m_btnPlay;
   QPushButton* m_btnStop;

   QLabel* m_lblPosition;
   QSlider* m_sldPosition;
   QLabel* m_lblDuration;

//
   GTimeSeek* m_timeSeek;
   GLowPass3* m_lpFilter;
   GSoundTouch* m_soundTouch;
   GLevelMeter* m_levelMeter;

   uint32_t m_state;
//   uint64_t m_frameIndex;
   int m_updateTimerID;

//   bool m_isLooping;
//   int m_loopIndex;
//   int m_loopCount;

   de::audio::Buffer m_sampleBuffer;
   de::audio::BufferPlayer m_bufferPlayer;

   std::string m_uri;


};
