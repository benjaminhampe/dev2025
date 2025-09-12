#include "QImageWidget.hpp"
#include <QResizeEvent>
QImageWidget::QImageWidget( QWidget* parent )
   : QWidget(parent)
   , m_isDisplayDirty( true )
   , m_imgScaling( true )
   , m_imgPreserveAspectWoH( false )
{
   setObjectName( "QImageWidget" );
   setContentsMargins( 0,0,0,0 );
}

QImageWidget::~QImageWidget()
{
}

void
QImageWidget::updateImage()
{
   if ( !m_isDisplayDirty ) return;

   if ( m_imgSource.isNull() )
   {
      m_imgDisplay = QImage();
   }
   else
   {
      int w = width();
      int h = height();
      if ( m_imgScaling )
      {
         if ( m_imgPreserveAspectWoH )
         {
            m_imgDisplay = m_imgSource.scaled( w,h, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation );
         }
         else
         {
            m_imgDisplay = m_imgSource.scaled( w,h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );
         }
      }
      else
      {
         m_imgDisplay = m_imgSource;
      }
   }

   m_isDisplayDirty = false;
}

void QImageWidget::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
   m_isDisplayDirty = true;
   update();
}

void QImageWidget::paintEvent( QPaintEvent* event )
{
   updateImage();

   int w = width();
   int h = height();

   if ( w > 1 && h > 1 )
   {
      int img_w = m_imgDisplay.width();
      int img_h = m_imgDisplay.height();
      if ( img_w > 0 && img_h > 0 )
      {
         QPainter dc( this );
         //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
         //dc.setCompositionMode( QPainter::CompositionMode_SourceOver );
         int x = (w - img_w)/2;
         int y = (h - img_h)/2;
         dc.drawImage( x,y, m_imgDisplay );
      }
   }

   QWidget::paintEvent( event );
}
