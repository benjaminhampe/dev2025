#include "GADSR.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GADSR::GADSR( QWidget* parent )
    : QWidget(parent)
    //, m_isBypassed( false )
    , m_adsr( nullptr )
{
   setObjectName( "GADSR" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("Env", this);
   m_A = new GSliderV( "A", 80, 0, 1000, "ms", 1, this );
   m_D = new GSliderV( "D", 80, 0, 1000, "ms", 1, this );
   m_S = new GSliderV( "S", 20, 0, 100, "%", 1, this );
   m_R = new GSliderV( "R", 200, 0, 4000, "ms", 1, this );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 3 );
   h->addWidget( m_menu );
   h->addWidget( m_A );
   h->addWidget( m_D );
   h->addWidget( m_S );
   h->addWidget( m_R );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_menuButton_hideAll(bool)) );
   connect( m_A, SIGNAL(valueChanged(int)), this, SLOT(on_sliderAttackTime(int)) );
   connect( m_D, SIGNAL(valueChanged(int)), this, SLOT(on_sliderDecayTime(int)) );
   connect( m_S, SIGNAL(valueChanged(int)), this, SLOT(on_sliderSustainLevel(int)) );
   connect( m_R, SIGNAL(valueChanged(int)), this, SLOT(on_sliderReleaseTime(int)) );
}

GADSR::~GADSR()
{}

void
GADSR::setADSR( de::audio::ADSR* adsr )
{
   m_adsr = adsr;
   if ( m_adsr )
   {
      int a = m_adsr->m_a;
      int d = m_adsr->m_d;
      int s = 100.0f * m_adsr->m_s;
      int r = m_adsr->m_r;
      m_A->setValue( a );
      m_D->setValue( d );
      m_S->setValue( s );
      m_R->setValue( r );
      DE_DEBUG("GADSR.setADSR(A:",a,", D:",d,", S:",s,", R:",r,")")
   }
}

void
GADSR::on_menuButton_bypass(bool checked)
{
   if ( m_adsr )
   {
      m_adsr->setBypassed( checked );
   }
}

void
GADSR::on_menuButton_more(bool checked)
{
   //m_qpeak->setVisible( checked );
   //m_volume->setVisible( checked );
}

void
GADSR::on_menuButton_hideAll(bool checked)
{
   m_A->setVisible( !checked );
   m_D->setVisible( !checked );
   m_S->setVisible( !checked );
   m_R->setVisible( !checked );
}

void
GADSR::on_sliderAttackTime( int v )
{
   if ( !m_adsr ) { DE_ERROR("No ADSR") return; }
   m_adsr->setA( std::clamp( v, 0, 2000 ) );
   //DE_DEBUG( m_adsr->toString() )
}

void
GADSR::on_sliderDecayTime( int v )
{
   if ( !m_adsr ) { DE_ERROR("No ADSR") return; }
   m_adsr->setD( std::clamp( v, 0, 2000 ) );
   //DE_DEBUG( m_adsr->toString() )
}

void
GADSR::on_sliderSustainLevel( int v )
{
   if ( !m_adsr ) { DE_ERROR("No ADSR") return; }
   m_adsr->setS( 0.01f * std::clamp( v, 1, 100 ) );
   //DE_DEBUG( m_adsr->toString() )
}

void
GADSR::on_sliderReleaseTime( int v )
{
   if ( !m_adsr ) { DE_ERROR("No ADSR") return; }
   m_adsr->setR( std::clamp( v, 1, 16000 ) );
   //DE_DEBUG( m_adsr->toString() )
}
