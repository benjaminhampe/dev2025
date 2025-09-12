#include "GGroupV.hpp"
#include <QVBoxLayout>

namespace {
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
}

GGroupV::GGroupV( QString name, QWidget* parent )
   : QWidget(parent)
   , m_body( nullptr )
{
   setObjectName( "GGroupV" );
   setContentsMargins( 0,0,0,0 );
   setMinimumWidth(24);
   setMaximumWidth(24);

   // Create Image
   auto ts = m_font.getTextSize( name );
   int w = ts.width()+2;
   int h = ts.height()+2;
   QImage img( w, h, QImage::Format_ARGB32 );
   img.fill( QColor(255,255,255,255) );

   m_font.drawText( img, 1,1, name, QColor(0,0,0) );

   m_title = new QImageWidget( this );
   m_title->setImage( rotateLeft( img ), true );
   m_title->setImageScaling( false );

   m_btnMinified = new QPushButton( "M", this );
   m_btnMinified->setMinimumSize( 20,20 );
   m_btnMinified->setMaximumSize( 20,20 );
   m_btnMinified->setToolTip( "This DSP element is fully visible now and (s)hown." );
   m_btnMinified->setCheckable( true );
   m_btnMinified->setChecked( true );

   m_btnBypassed = new QPushButton( "E", this );
   m_btnBypassed->setMinimumSize( 20,20 );
   m_btnBypassed->setMaximumSize( 20,20 );
   m_btnBypassed->setToolTip("This DSP element is now (e)nabled = not bypassed");
   m_btnBypassed->setCheckable( true );
   m_btnBypassed->setChecked( true );

   m_btnExtraMore = new QPushButton( "-", this );
   m_btnExtraMore->setMinimumSize( 20,20 );
   m_btnExtraMore->setMaximumSize( 20,20 );
   m_btnExtraMore->setToolTip("All DSP options are visible now");
   m_btnExtraMore->setCheckable( true );
   m_btnExtraMore->setChecked( true );

   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins( 1,1,1,1 );
   v->setSpacing( 1 );
   v->addWidget( m_btnBypassed );
   v->addStretch( 1 );
   v->addWidget( m_title );
   v->addStretch( 1 );
   v->addWidget( m_btnExtraMore );
   v->addWidget( m_btnMinified );
   v->setAlignment( m_btnBypassed, Qt::AlignHCenter );
   v->setAlignment( m_title, Qt::AlignHCenter );
   v->setAlignment( m_btnExtraMore, Qt::AlignHCenter );
   v->setAlignment( m_btnMinified, Qt::AlignHCenter );
   setLayout( v );
   connect( m_btnBypassed, SIGNAL(toggled(bool)), this, SLOT(setBypassed(bool)) );
   connect( m_btnExtraMore, SIGNAL(toggled(bool)), this, SLOT(setExtraMore(bool)) );
   connect( m_btnMinified, SIGNAL(toggled(bool)), this, SLOT(setMinified(bool)) );
}

GGroupV::~GGroupV()
{
}
