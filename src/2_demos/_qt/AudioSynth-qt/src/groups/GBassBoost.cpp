#include "GBassBoost.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

GBassBoost::GBassBoost( QWidget* parent )
   : QWidget(parent)
{
   setObjectName( "GBassBoost" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("BASS BOOST", this);
   m_body = new QImageWidget(this);
   m_menu->setBody( m_body );

   //m_freq = new GSliderV( "LowPass3", 60,0,10000,"Hz",1, this );
   m_qpeak = new GSliderV( "QPeak", 200,0,1000,"",1, this );
   m_volume = new GSliderV( "Vol.", 100,0,200,"%",1, this );

   //setFrequency( float( m_freq->value() ) );
   m_lp.setFrequency( 60.0f );
   m_lp.setQ( 0.01f * float( m_qpeak->value() ) );
   m_lp.setVolume( m_volume->value() ); // Boost volume, can be over 100%, ca. 300%.
   m_lp.setInputVolume( 100 ); // Boost means additional to original signal

   QHBoxLayout* hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 3 );
   //h->addWidget( m_freq );
   hBody->addWidget( m_qpeak );
   hBody->addWidget( m_volume );
   m_body->setLayout( hBody );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 3 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_toggledBypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_toggledMore(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_toggledHideAll(bool)) );
   //connect( m_freq, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged_f(int)) );
   connect( m_qpeak, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged_Q(int)), Qt::QueuedConnection );
   connect( m_volume, SIGNAL(valueChanged(int)), this, SLOT(on_valueChanged_vol(int)), Qt::QueuedConnection );
}

GBassBoost::~GBassBoost()
{}

