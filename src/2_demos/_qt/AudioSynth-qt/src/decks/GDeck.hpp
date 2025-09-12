#pragma once
#include <GLevelMeterH.hpp>
#include <QPushButton>
#include <QFont5x8.hpp>
#include <DarkImage.h>
#include <de/audio/dsp/IDspChainElement.hpp>

// ============================================================================
class GDeck : public QWidget,
               public de::audio::IDspChainElement
// ============================================================================
{
   Q_OBJECT
public:
   GDeck( QString name, QWidget* parent = 0 );
   ~GDeck();

   uint64_t
   readDspSamples( double pts, float* dst,
      uint32_t frames, uint32_t channels, double rate ) override;

   bool isExtraMore() const { return m_btnExtraMore->isChecked(); }
   bool isBypassed() const {return m_btnBypassed->isChecked(); }
   bool isMinified() const { return m_btnMinified->isChecked(); }
signals:
   void toggledBypass(bool);
   void toggledMore(bool);
   void toggledMinimized(bool);
public slots:
   void clearInputSignals() override;
   void setInputSignal( int i, de::audio::IDspChainElement* input ) override;
   void setBackgroundColor( QColor bgColor );
   void setBody(QWidget* body);
   void setMinified(bool minimized);
   void setBypassed(bool bypassed);
   void setExtraMore(bool visible);
   void setTitleImageText( QString title );
//protected slots:
//   void startUpdateTimer();
//   void stopUpdateTimer();
protected:
//   void timerEvent( QTimerEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;
public:
   // === DSP stuff ===
   de::audio::IDspChainElement* m_inputSignal;
   //std::vector< float > m_inputBuffer;
   float m_Lmin;
   float m_Lmax;
   float m_Rmin;
   float m_Rmax;
   //   int m_updateTimerId;
   //   std::vector< float > m_accumBuffer;
   // === Widget stuff ===
   QWidget* m_body;
   QPushButton* m_btnMinified;
   QPushButton* m_btnBypassed;
   QPushButton* m_btnExtraMore;
   QImageWidget* m_title;
   GLevelMeterH* m_levelL;
   GLevelMeterH* m_levelR;

   QColor m_deckColor;
   QFont5x8 m_font5x8;
   de::LinearColorGradient m_ColorGradient;
};
