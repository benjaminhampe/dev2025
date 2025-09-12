#include "GDetuneSynth.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

using namespace de::audio; // DSP_RESIZE, DSP_FILLZERO, DSP_COPY

GDetuneSynth::GDetuneSynth( int index, QWidget* parent )
    : QWidget(parent)
    , m_index( index )
{
   QString synthName = QString("GDetuneSynth {%1}" ).arg(m_index);

//   QStringList osc_types;
//   osc_types << "1. Sinus"
//             << "2. Ellipse"
//             << "3. SawImpulse"
//             << "4. Saw"
//             << "5. Ramp"
//             << "6. Triangle"
//             << "7. Tri2RectMorph"
//             << "8. Rect"
//             << "9. Piano"
//             << "10. Noise";

   setObjectName( synthName );
   setContentsMargins(0,0,0,0);
   setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
   m_menu = new GDeck( synthName, this );
   m_body = new QImageWidget( this );
   m_menu->setBody( m_body );

   m_oscLabel = new QLabel( "OSC", this );
   m_oscEdit = new QComboBox( this );

   for ( int i = 0; i < int(de::audio::MultiOSCf::eTypeCount); ++i )
   {
      auto s = QString::fromStdString( de::audio::MultiOSCf::getFunctionString( i ) );
      m_oscEdit->addItem( s, QVariant(i) );
   }
   //m_oscEdit->addItems( osc_types );
   m_oscEdit->setCurrentIndex( 0 );

   m_noteLabel = new QLabel( "Note", this );
   m_noteEdit = new QSpinBox( this );
   m_noteEdit->setMinimum( -100 );
   m_noteEdit->setMaximum( 100 );
   m_noteEdit->setValue( 0 );
   m_noteEdit->setSingleStep( 1 );

   m_noteDetuneLabel = new QLabel( "Detune", this );
   m_noteDetuneEdit = new QSpinBox( this );
   m_noteDetuneEdit->setMinimum( -100 );
   m_noteDetuneEdit->setMaximum( 100 );
   m_noteDetuneEdit->setValue( 0 );
   m_noteDetuneEdit->setSingleStep( 1 );

   auto f = new QFormLayout();
   f->addRow( m_oscLabel, m_oscEdit );
   f->addRow( m_noteLabel, m_noteEdit );
   f->addRow( m_noteDetuneLabel, m_noteDetuneEdit );

   m_lfoPhase = new GLFO( "Phase", this );
   m_lfoPulseWidth = new GLFO( "PulseWid", this );
   m_lfoDetune = new GLFO( "Detune", this );
   m_adsr = new GADSR( this );
   m_lowPass3 = new GLowPass3( this );
   m_levelMeter = new GLevelMeter( this );
   m_volumeSlider = new GVolume( "P1-Vol.", this );
   m_waveform = new GOszilloskop( this );

   auto h = new QHBoxLayout();
   h->setContentsMargins(0,0,0,0);
   h->setSpacing( 3 );
   h->addLayout( f );
   h->addWidget( m_lfoPhase );
   h->addWidget( m_lfoPulseWidth );
   h->addWidget( m_lfoDetune );
   h->addWidget( m_adsr );
   h->addWidget( m_lowPass3 );
   h->addWidget( m_waveform,1 );
   h->addWidget( m_volumeSlider );
   h->addWidget( m_levelMeter );
//   h->addStretch( 1 );
   m_body->setLayout( h );

   auto mainLayout = new QVBoxLayout();
   mainLayout->setContentsMargins(0,0,0,0);
   mainLayout->setSpacing( 0 );
   mainLayout->addWidget( m_menu );
   mainLayout->addWidget( m_body,1 );
   setLayout( mainLayout );

   m_lfoPhase->setValues( 0, 1, 0, 0 );
   m_lfoPulseWidth->setValues( 0, 1, 0.5, 0 );
   m_lfoDetune->setValues( 0, 1, 0, 0 );
   m_waveform->setShiftBufferSize( 1024 );
   m_synth.setupDspElement( 1024, 2, 48000 );

   m_lfoPhase->setLFO( &m_synth.m_lfoPhase );
   m_lfoPulseWidth->setLFO( &m_synth.m_lfoPulseWidth );
   m_lfoDetune->setLFO( &m_synth.m_lfoDetune );
   m_adsr->setADSR( &m_synth.m_adsr );
   //m_lfo1->setInputSignal( m_piano );
   //m_delay->setInputSignal( m_piano );
   //m_delay->setBypassed( true );
   m_lowPass3->dsp()->setInputSignal( 0, &m_synth );

   m_volumeSlider->setInputSignal( 0, m_lowPass3->dsp() );

   m_levelMeter->dsp()->setInputSignal( 0, m_volumeSlider );

   m_waveform->setInputSignal( 0, m_levelMeter->dsp() );

   m_menu->setInputSignal( 0, m_waveform );

   connect( m_oscEdit, SIGNAL(currentIndexChanged(int)),
                 this, SLOT(on_oscTypeChanged(int)) );

//   connect( m_lfoPhase.minEdit, SIGNAL(valueChanged(int)),
//            this, SLOT(on_lfoRateChanged(int)), Qt::QueuedConnection );

//   connect( m_min.slider, SIGNAL(valueChanged(int)),
//            this, SLOT(on_lfoMinChanged(int)), Qt::QueuedConnection );

//   connect( m_max.slider, SIGNAL(valueChanged(int)),
//            this, SLOT(on_lfoMaxChanged(int)), Qt::QueuedConnection );

   m_lfoPhase->setMinified( true );
   m_lfoPhase->setBypassed( true );

   m_lfoPulseWidth->setMinified( true );
   m_lfoPulseWidth->setBypassed( true );

   m_lfoDetune->setMinified( true );
   m_lfoDetune->setBypassed( true );

   m_adsr->setMinified( true );
   m_adsr->setBypassed( false );

   m_lowPass3->setMinified( true );
   m_lowPass3->setBypassed( true );
}

GDetuneSynth::~GDetuneSynth()
{
   // stopUpdateTimer();
}

void
GDetuneSynth::sendNote( de::audio::Note const & note )
{
   // Use given note to copy to all active detuned oscs.
   // This pollutes with many notes but gives detune control for all osc's.
   de::audio::Note enote = note;
   enote.m_channel = 0;
   enote.m_midiNote += m_noteEdit->value();
   enote.m_detune += 0.01 *m_noteDetuneEdit->value();
   m_synth.sendNote( enote );
}

uint64_t
GDetuneSynth::readDspSamples(
   double pts,
   float* dst,
   uint32_t frames,
   uint32_t channels,
   double sampleRate )
{
   return m_menu->readDspSamples( pts, dst, frames, channels, sampleRate );
}

void
GDetuneSynth::on_oscTypeChanged( int v )
{
   DetuneSynth::setOscType( m_synth.m_osc, MultiOSCf::eType(v) );
}

//void
//GDetuneSynth::on_toneSine( int v )
//{
//   int toneOffset = m_sineDetuneSlider->value();
//   int sign = toneOffset < 0 ? -1:1;
//   toneOffset = abs( toneOffset );
//   getFrequencyFromMidiNote( toneOffset )
//   m_sineOsc.setFrequencyOffset( );;
//}

/*
void
GDetuneSynth::on_volumeSine1( int v )
{
   int vol = std::clamp( v, 0, 100 );
   m_sine1.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSine1.setVolume( vol );
}

void
GDetuneSynth::on_detuneSine1( int v )
{
   //int offset = m_sine1.detuneSlider->value();
   m_sine1.detune = v;
   m_sine1.detuneAmount->setText( QString::number(v) );
}

void
GDetuneSynth::on_volumeSine2( int v )
{
   int vol = std::clamp( v, 0, 100 ); // m_sine2.volumeSlider->value()
   m_sine2.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSine2.setVolume( vol );
}

void
GDetuneSynth::on_detuneSine2( int v )
{
   m_sine2.detune = v;
   m_sine2.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeSine3( int v )
{
   int vol = std::clamp( v, 0, 100 ); // m_sine3.volumeSlider->value()
   m_sine3.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSine3.setVolume( vol );
}

void
GDetuneSynth::on_detuneSine3( int v )
{
   m_sine3.detune = v;
   m_sine3.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeSine4( int v )
{
   int vol = std::clamp( v, 0, 100 ); // m_sine4.volumeSlider->value()
   m_sine4.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSine4.setVolume( vol );
}

void
GDetuneSynth::on_detuneSine4( int v )
{
   m_sine4.detune = v;
   m_sine4.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeSine5( int v )
{
   int vol = std::clamp( v, 0, 100 ); // m_sine5.volumeSlider->value()
   m_sine5.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSine5.setVolume( vol );
}

void
GDetuneSynth::on_detuneSine5( int v )
{
   m_sine5.detune = v;
   m_sine5.detuneAmount->setText( QString::number(v) ); // offset
}
void
GDetuneSynth::on_volumeTriangle( int v )
{
   int vol = std::clamp( m_tri.volumeSlider->value(), 0, 100 );
   m_tri.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscTriangle.setVolume( vol );
}
void
GDetuneSynth::on_detuneTriangle( int v )
{
   m_tri.detune = v;
   m_tri.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeRect( int v )
{
   int vol = std::clamp( m_rect.volumeSlider->value(), 0, 100 );
   m_rect.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscRect.setVolume( vol );
}

void
GDetuneSynth::on_detuneRect( int v )
{
   m_rect.detune = v;
   m_rect.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeSaw( int v )
{
   int vol = std::clamp( m_saw.volumeSlider->value(), 0, 100 );
   m_saw.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscSaw.setVolume( vol );
}

void
GDetuneSynth::on_detuneSaw( int v )
{
   m_saw.detune = v;
   m_saw.detuneAmount->setText( QString::number(v) ); // offset
}

void
GDetuneSynth::on_volumeNoise( int v )
{
   int vol = std::clamp( m_noise.volumeSlider->value(), 0, 100 );
   m_noise.volumeAmount->setText( QString::number(vol) + " % " );
   m_oscNoise.setVolume( vol );
}

void
GDetuneSynth::on_detuneNoise( int v )
{
   m_noise.detune = v;
   m_noise.detuneAmount->setText( QString::number(v) ); // offset
}
*/
