#include "GLimiter.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GLimiter::GLimiter( QWidget* parent )
    : QWidget(parent)
{
   setObjectName( "GLimiter" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("Limiter", this);
   m_body = new QImageWidget(this);
   m_menu->setBody( m_body );

   m_attackTime = new GSliderV( "Vol.",10,1,800,"ms",1, this );
   m_attackTime->setValue( int(m_limiter.m_attackInMs) );

   m_releaseTime = new GSliderV( "Clean", 500,1,2000,"ms",1, this );
   m_releaseTime->setValue( int(m_limiter.m_releaseInMs) );

   m_gain = new GSliderV( "Gain", 100,0,200,"%",1, this );

   QHBoxLayout* hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 0 );
   hBody->addWidget( m_attackTime );
   hBody->addWidget( m_releaseTime );
   hBody->addWidget( m_gain );
   //hBody->addWidget( m_blendKnob );
   m_body->setLayout( hBody );

   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_menuButton_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_menuButton_more(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_menuButton_hideAll(bool)) );

   connect( m_attackTime, SIGNAL(valueChanged(int)), this, SLOT(on_attackTime(int)), Qt::QueuedConnection );
   connect( m_releaseTime, SIGNAL(valueChanged(int)), this, SLOT(on_releaseTime(int)), Qt::QueuedConnection );
   connect( m_gain, SIGNAL(valueChanged(int)), this, SLOT(on_gain(int)), Qt::QueuedConnection );

}

GLimiter::~GLimiter()
{}

void GLimiter::on_menuButton_hideAll(bool checked)
{
//   if ( checked )
//   {
//      m_attackTime->setVisible( false );
//      m_releaseTime->setVisible( false );
//      m_gain->setVisible( false );

//   }
//   else
//   {
//      m_attackTime->setVisible( true );
//      m_releaseTime->setVisible( true );
//      if ( m_menu->isExtraMore() )
//      {
//         m_gain->setVisible( true );
//      }
//   }
}

void GLimiter::on_menuButton_bypass(bool checked)
{
   m_limiter.setBypassed( checked );
}

void GLimiter::on_menuButton_more(bool checked)
{
   m_gain->setVisible( checked );
}

void
GLimiter::on_attackTime( int v )
{
   DE_DEBUG("value = ",v)
   m_limiter.setAttackTime( v );
}

void
GLimiter::on_releaseTime( int v )
{
   DE_DEBUG("value = ",v)
   m_limiter.setReleaseTime( v );
}

void
GLimiter::on_gain( int v )
{
   DE_DEBUG("value = ",v)
   //m_limiter.setGain( v );
}
