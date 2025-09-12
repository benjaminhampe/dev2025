#include "GVerticalText.hpp"
#include <QVBoxLayout>

QImage
rotateLeft( QImage const & img )
{
   int w = img.width();
   int h = img.height();
   QImage dst( h, w, img.format() );

   for ( int32_t y = 0; y < h; ++y )
   {
      for ( int32_t x = 0; x < w; ++x )
      {
         uint32_t color = img.pixel( x,y );
         dst.setPixel( y, w - 1 - x, color);
      }
   }

   return dst;
}


GVerticalText::GVerticalText( QString name, QWidget* parent )
   : QImageWidget(parent)
{
   setObjectName( "GVerticalText" );
   setContentsMargins( 0,0,0,0 );

   setMinimumWidth(24);
   setMaximumWidth(24);
   setMinimumHeight(64);

   int w = 64;
   int h = 24;
   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( QColor(255,255,255,255) );

   auto ts = m_font.getTextSize( name );
   int tw = ts.width();
   int th = ts.height();
   int x = 1;
   int y = h / 2;

   m_font.drawText( img, x,y, name, QColor(0,0,0), de::Align::LeftMiddle );

//   QPainter dc;
//   if ( dc.begin( &img ) )
//   {
//      QFont font0 = dc.font();
//      dc.setBrush( QBrush( Qt::black ) );
//      dc.setPen( QPen( QColor( Qt::black ), 3.5f ) );
//      dc.setFont( QFont( "Calibri", 8 ) );
//      QSize ts = dc.fontMetrics().size( Qt::TextDontClip, name );
//      int tw = ts.width();
//      int th = ts.height();
//      int x = (w - tw) / 2;
//      int y = 16;

//      dc.drawText( x, y, name );
//   }

   setImage( rotateLeft( img ), true );
   setImageScaling( false );

   m_bypass = new QPushButton( "O", this );
   m_bypass->setCheckable( true );
   m_bypass->setChecked( false );
   m_bypass->setMinimumSize( 20,20 );
   m_bypass->setMaximumSize( 20,20 );

   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins( 2,2,2,2 );
   v->addWidget( m_bypass );
   v->addStretch( 1 );
   setLayout( v );
}

GVerticalText::~GVerticalText()
{
}
