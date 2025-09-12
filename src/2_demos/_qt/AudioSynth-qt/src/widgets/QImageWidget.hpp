#ifndef DE_Q_IMAGEWIDGET_HPP
#define DE_Q_IMAGEWIDGET_HPP

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

// ============================================================================
class QImageWidget : public QWidget
// ============================================================================
{
   Q_OBJECT
   //DE_CREATE_LOGGER("QImageWidget")
public:
   QImageWidget( QWidget* parent = 0 );
   ~QImageWidget();
signals:
public slots:
   void setImage( QImage img, bool useAsMinSize = false )
   {
      m_imgSource = img;
      int w = m_imgSource.width();
      int h = m_imgSource.height();
      if ( w < 1 || h < 1 )
      {
         setVisible( false );
      }
      else
      {
         setVisible( true );
         if ( useAsMinSize )
         {
            setMinimumSize( w,h );
         }
      }

      //setMinimumSize( std::min(w,64),std::min(h,64) );

      m_isDisplayDirty = true;
      update();
   }
   void setImageScaling( bool enabled ) { m_imgScaling = enabled; }
   void setImagePreserveAspectWoH( bool enabled ) { m_imgPreserveAspectWoH = enabled; }

protected:
   void updateImage();
   void resizeEvent( QResizeEvent* event ) override;
   void paintEvent( QPaintEvent* event ) override;

private:
   QImage m_imgSource;
   QImage m_imgDisplay;
   bool m_isDisplayDirty;
   bool m_imgScaling;
   bool m_imgPreserveAspectWoH;
};

#endif // DE_Q_IMAGEWIDGET_HPP
