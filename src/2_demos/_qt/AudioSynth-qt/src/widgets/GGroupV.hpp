#pragma once
#include "QImageWidget.hpp"
#include <QPushButton>
#include <QFont5x8.hpp>
// ============================================================================
class GGroupV : public QWidget
// ============================================================================
{
   Q_OBJECT
protected:
   QFont5x8 m_font;
   QWidget* m_body;
   QImageWidget* m_title;
   QPushButton* m_btnBypassed;
   QPushButton* m_btnExtraMore;
   QPushButton* m_btnMinified;

   void updateBody()
   {
      if ( m_body )
      {
         m_body->setVisible( isMinified() );
      }
   }
public:
   GGroupV( QString name, QWidget* parent = 0 );
   ~GGroupV();
   bool isMinified() const { return m_btnMinified->isChecked(); }
   bool isBypassed() const { return m_btnBypassed->isChecked(); }
   bool isExtraMore() const { return m_btnExtraMore->isChecked(); }
signals:
   void toggledHideAll(bool);
   void toggledBypass(bool);
   void toggledMore(bool);
public slots:
   void setBody( QWidget* body )
   {
      m_body = body;
      updateBody();
   }

   void setMinified(bool minified)
   {
      if ( minified )
      {
         //m_btnMinified->blockSignals( true );
         m_btnMinified->setText("M");
         m_btnMinified->setToolTip("Minified");
         //m_btnMinified->blockSignals( false );

         //m_btnExtraMore->blockSignals( true );
         m_btnExtraMore->setVisible( false );
         //m_btnExtraMore->blockSignals( false );
         if ( m_body )
         {
            //m_body->blockSignals( true );
            m_body->setVisible( false );
            //m_body->blockSignals( false );
         }
      }
      else
      {
         //m_btnMinified->blockSignals( true );
         m_btnMinified->setText("F");
         m_btnMinified->setToolTip("Fully visible");
         //m_btnMinified->blockSignals( false );

         //m_btnExtraMore->blockSignals( true );
         m_btnExtraMore->setVisible( true );
         //m_btnExtraMore->blockSignals( false );

         if ( m_body )
         {
            //m_body->blockSignals( true );
            m_body->setVisible( true );
            //m_body->blockSignals( false );
         }
      }

      //dbDebug("GGroupV.setMinified(",minified,")")
      emit toggledHideAll( minified );
   }

   void setBypassed(bool bypassed)
   {
      if ( bypassed )
      {
         //m_btnBypassed->blockSignals( true );
         m_btnBypassed->setText("B");
         m_btnBypassed->setToolTip("This DSP element is DISABLED ( bypassed )");
         //m_btnBypassed->blockSignals( false );
      }
      else
      {
         //m_btnBypassed->blockSignals( true );
         m_btnBypassed->setText("E");
         m_btnBypassed->setToolTip("This DSP element is ENABLED");
         //m_btnBypassed->blockSignals( false );
      }

      //dbDebug("GGroupV.setBypassed(",bypassed,")")
      emit toggledBypass( bypassed );


   }
   void setExtraMore(bool visible)
   {
      if ( visible )
      {
         m_btnExtraMore->blockSignals( true );
         m_btnExtraMore->setText("-");
         m_btnExtraMore->setToolTip("Press to show less");
         m_btnExtraMore->blockSignals( false );
      }
      else
      {
         m_btnExtraMore->blockSignals( true );
         m_btnExtraMore->setText("+");
         m_btnExtraMore->setToolTip("Press to show more");
         m_btnExtraMore->blockSignals( false );
      }

      //dbDebug("GGroupV.setExtraMore(",visible,")")
      emit toggledMore( visible );

   }

};
