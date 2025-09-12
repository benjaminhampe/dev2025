#include "GFileInfo.hpp"

#include <de/audio/GetCoverArt.hpp>
#include <QString>

GFileInfo::GFileInfo( QWidget* parent )
   : QWidget( parent )
{
   setObjectName( "GFileInfo" );

   m_edtUri = new QLineEdit( "Unloaded.nothing", this );
   m_btnLoad = new QPushButton( "...", this );
   m_btnPlay = new QPushButton( "Play", this );
   m_btnStop = new QPushButton( "Stop", this );

   m_lblPosition = new QLabel( "-:-", this );
   m_sldPosition = new QSlider( Qt::Horizontal, this );
   m_sldPosition->setMinimum( 0 );
   m_sldPosition->setMaximum( 1255 );
   m_sldPosition->setValue( 0 );
   m_lblDuration = new QLabel( "-:-", this );

   m_coverArt = new QImageWidget( this );
   m_coverArt->setMinimumSize(200,200);
   m_coverArt->setMaximumSize(200,200);
   m_coverArt->setVisible( false );

   QHBoxLayout* h1 = new QHBoxLayout();
   h1->setContentsMargins(0,0,0,0);
   h1->setSpacing( 5 );
   h1->addWidget( m_edtUri );
   h1->addWidget( m_btnLoad );

   QHBoxLayout* h2 = new QHBoxLayout();
   h2->setContentsMargins(0,0,0,0);
   h2->setSpacing( 5 );
   h2->addWidget( m_btnPlay );
   h2->addWidget( m_btnStop );

   QHBoxLayout* h3 = new QHBoxLayout();
   h3->setContentsMargins(0,0,0,0);
   h3->setSpacing( 5 );
   h3->addWidget( m_lblPosition );
   h3->addWidget( m_sldPosition );
   h3->addWidget( m_lblDuration );

   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 5 );
   v->addLayout( h1 );
   v->addLayout( h2 );
   v->addLayout( h3 );

   QHBoxLayout* h4 = new QHBoxLayout();
   h4->setContentsMargins(0,0,0,0);
   h4->setSpacing( 5 );
   h4->addWidget( m_coverArt );
   h4->addLayout( v );
   setLayout( h4 );

//   connect( m_btnLoad, SIGNAL(clicked(bool)), this, SLOT(on_clicked_load(bool)), Qt::QueuedConnection );
//   connect( m_btnPlay, SIGNAL(clicked(bool)), this, SLOT(on_clicked_play(bool)), Qt::QueuedConnection );
//   connect( m_btnStop, SIGNAL(clicked(bool)), this, SLOT(on_clicked_stop(bool)), Qt::QueuedConnection );
//   connect( m_sldPosition, SIGNAL(valueChanged(int)), this, SLOT(on_slider_pos(int)), Qt::QueuedConnection );
}

GFileInfo::~GFileInfo()
{

}

void
GFileInfo::load()
{

}

//void
//GFileInfo::on_slider_pos( int value )
//{
//   typedef double T;
//   int v = value; // m_sldPosition->value();
//   int m = m_sldPosition->minimum();
//   int n = m_sldPosition->maximum();
//   T t = T( v - m ) / T( n - m );

//   m_frameIndex = t * double(m_sampleBuffer.getFrameCount());

//   T dur = t * m_sampleBuffer.getDuration();
//   T pos = t * dur;

//   m_lblPosition->setText( QString::fromStdString( dbStrSeconds( pos ) ) );
//   m_lblDuration->setText( QString::fromStdString( dbStrSeconds( dur ) ) );
//}

//void
//GFileInfo::on_clicked_load(bool checked)
//{
//   load();
//}

//void
//GFileInfo::on_clicked_play(bool checked)
//{
//   play();
//   m_edtUri->setText( QString::fromStdString( m_sampleBuffer.getUri() ) );
//}

//void
//GFileInfo::on_clicked_stop(bool checked)
//{
//   stop();
//}
