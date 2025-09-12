#include "GCompressor.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
using namespace de::audio;

GCompressor::GCompressor( QWidget* parent )
   : QWidget(parent)
{
   setObjectName( "GCompressor" );
   setContentsMargins(0,0,0,0);

   m_menu = new GGroupV("Compressor", this);
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_threshKnob = new GSliderV( "Threshold", 0,0,100,"*",1 , this );
   m_ratioKnob = new GSliderV( "Ratio",20,11,160,"2:1",1, this );
   m_attackKnob = new GSliderV( "Attack", 5,0,200,"ms",1, this );
   m_releaseKnob = new GSliderV( "Release", 5,0,200,"ms",1, this );
   m_gainKnob = new GSliderV( "Gain", 100,0,200,"%",1, this );

   QHBoxLayout* hBody = new QHBoxLayout();
   hBody->setContentsMargins(0,0,0,0);
   hBody->setSpacing( 0 );
   hBody->addWidget( m_threshKnob );
   hBody->addWidget( m_ratioKnob );
   hBody->addWidget( m_attackKnob );
   hBody->addWidget( m_releaseKnob );
   hBody->addWidget( m_gainKnob );
   m_body->setLayout( hBody );

   QHBoxLayout* h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 0 );
   h->addWidget( m_menu );
   h->addWidget( m_body );
   setLayout( h );

   connect( m_menu, SIGNAL(toggledBypass(bool)), this, SLOT(on_btnToggle_bypass(bool)) );
   connect( m_menu, SIGNAL(toggledMore(bool)), this, SLOT(on_btnToggle_extraMore(bool)) );
   connect( m_menu, SIGNAL(toggledHideAll(bool)), this, SLOT(on_btnToggle_hideAll(bool)) );
   connect( m_threshKnob, SIGNAL(valueChanged(int)), this, SLOT(on_thresholdKnob(int)) );
   connect( m_ratioKnob, SIGNAL(valueChanged(int)), this, SLOT(on_ratioKnob(int)) );
   connect( m_attackKnob, SIGNAL(valueChanged(int)), this, SLOT(on_attackKnob(int)) );
   connect( m_releaseKnob, SIGNAL(valueChanged(int)), this, SLOT(on_releaseKnob(int)) );
   connect( m_gainKnob, SIGNAL(valueChanged(int)), this, SLOT(on_gainKnob(int)) );

}

GCompressor::~GCompressor()
{
}

void GCompressor::on_btnToggle_hideAll(bool checked)
{
//   m_threshKnob->setVisible( false );
//   m_ratioKnob->setVisible( false );
//   m_attackKnob->setVisible( false );
//   m_releaseKnob->setVisible( false );
//   m_gainKnob->setVisible( false );
}

void GCompressor::on_btnToggle_bypass(bool checked)
{
   m_compressor.setBypassed( checked );
}

void GCompressor::on_btnToggle_extraMore(bool checked)
{
}

void GCompressor::on_thresholdKnob( int v )
{
   DE_DEBUG("value = ",v)
   m_compressor.setThreshold( 0.01f * v );
}

void GCompressor::on_ratioKnob( int v )
{
   DE_DEBUG("value = ",v)
   m_compressor.setRatio( 0.1f * v );
}

void GCompressor::on_attackKnob( int v )
{
   DE_DEBUG("value = ",v)
   m_compressor.setAttackTime( v );
}

void GCompressor::on_releaseKnob( int v )
{
   DE_DEBUG("value = ",v)
   m_compressor.setReleaseTime( v );
}
void GCompressor::on_gainKnob( int v )
{
   DE_DEBUG("value = ",v)
   m_compressor.setOutputGain( 0.01f * v );
}


//void
//GCompressor::clearInputSignals()
//{
//   m_compressor.clearInputSignals();
//}

//void
//GCompressor::setInputSignal( int i, de::audio::IDspChainElement* input )
//{
//   m_compressor.setInputSignal( i, input );
//}

//uint64_t
//GCompressor::readSamples( double pts, float* dst, uint32_t dstFrames, uint32_t dstChannels, uint32_t dstRate )
//{
//   return m_compressor.readSamples( pts, dst, dstFrames, dstChannels, dstRate );
//}
