#include "GSliderV.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>

GSliderV::GSliderV( QString name, int value, int min, int max, QString unit, int step, QWidget* parent )
   : QWidget( parent )
   , m_default( value )
   , m_unit( unit )
{
   setObjectName( "GSliderV" );
   setContentsMargins(0,0,0,0);

   m_title = new QLabel( name, this );
   m_slider = new QSlider( this );
   m_slider->setMinimumHeight( 64 );
   m_slider->setMinimum( min );
   m_slider->setMaximum( max );
   m_slider->setValue( value );
   m_slider->setSingleStep( step );

   QString amount = QString("%1%2").arg(m_slider->value()).arg(m_unit);
   m_amount = new QLabel( amount, this );

   QVBoxLayout* v = new QVBoxLayout();
   v->setContentsMargins(0,0,0,0);
   v->setSpacing( 3 );
   v->addWidget( m_title );
   v->addWidget( m_slider,1 );
   v->addWidget( m_amount );
   v->setAlignment( m_title, Qt::AlignHCenter );
   v->setAlignment( m_slider, Qt::AlignHCenter );
   v->setAlignment( m_amount, Qt::AlignHCenter );
   setLayout( v );

   connect( m_slider, SIGNAL(valueChanged(int)),
            this, SLOT(on_sliderChanged(int)), Qt::QueuedConnection );

}

GSliderV::~GSliderV()
{
}

void
GSliderV::on_sliderChanged( int v )
{
   QString s = QString::number(v);
   if ( !m_unit.isEmpty() )
   {
      s += m_unit;
   }
   m_amount->blockSignals( true );
   m_amount->setText( s );
   m_amount->blockSignals( false );
   emit valueChanged( v );
}



void
GSliderV::timerEvent( QTimerEvent* event )
{
   QWidget::timerEvent( event );
//   if ( event->timerId() == m_timerId )
//   {
//      update();
//   }
}

void
GSliderV::paintEvent( QPaintEvent* event )
{
   QWidget::paintEvent( event );
}
void
GSliderV::resizeEvent( QResizeEvent* event )
{
   QWidget::resizeEvent( event );
}

void
GSliderV::mouseMoveEvent( QMouseEvent* event )
{
   QWidget::mouseMoveEvent( event );
}

void
GSliderV::mousePressEvent( QMouseEvent* event )
{
   if ( event->button() == Qt::RightButton )
   {
      m_slider->setValue( m_default );
   }

   QWidget::mousePressEvent( event );
}

void
GSliderV::mouseReleaseEvent( QMouseEvent* event )
{
   QWidget::mouseReleaseEvent( event );
}


void
GSliderV::wheelEvent( QWheelEvent* event )
{
   QWidget::wheelEvent( event );
}

void
GSliderV::keyPressEvent( QKeyEvent* event )
{

//   auto key = event->key();

//   int k = 59; // 59;
//   if ( key == Qt::Key_1 ) { noteOn( k ); } k++;

//   event->accept();

   QWidget::keyPressEvent( event );
}

void
GSliderV::keyReleaseEvent( QKeyEvent* event )
{
   QWidget::keyReleaseEvent( event );
}



// void
// GSliderV::on_sliderPressed()
// {
   // DE_DEBUG("")
// }

// void
// GSliderV::on_sliderReleased()
// {
   // DE_DEBUG("")
// }

/*
void
GSliderV::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_updateTimerId )
   {
      update();
   }
}

// ============================================================================
void GSliderV::paintEvent( QPaintEvent* event )
// ============================================================================
{
   QPainter dc(this);
   dc.fillRect( rect(), QColor( 25,25,25 ) );

   int w = width();
   int h = height();
   int h2 = height() / 2;

   uint64_t srcSamples = m_shiftBuffer.size();
   uint64_t srcFrames = srcSamples / m_channelCount;
   uint64_t srcChannels = m_channelCount;
   if (srcFrames < 2) { return; }

   float const dx = float(w) / float(srcFrames);
   float const dy = float(h) * 0.5f;

   dc.setBrush( Qt::NoBrush );
   dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

   for ( uint64_t c = 0; c < srcChannels; ++c )
   {
      if ( c == 0 )
         dc.setPen( QPen( QColor(255,55,55,63) ) );
      else if ( c == 1 )
         dc.setPen( QPen( QColor(255,225,55,63) ) );
      else if ( c == 2 )
         dc.setPen( QPen( QColor(55,200,55,63) ) );
      else
         dc.setPen( QPen( QColor(255,255,255,63) ) );

      for ( uint64_t i = 1; i < srcFrames; ++i )
      {
         const float sample1 = m_shiftBuffer[ srcChannels*(i-1) + c ];
         const float sample2 = m_shiftBuffer[ srcChannels*(i) + c ];
         const int x1 = dx * (i-1);
         const int x2 = dx * (i);
         const int y1 = dy - dy * sample1;
         const int y2 = dy - dy * sample2;
         dc.drawLine( x1,y1,x2,y2 );
      }
   }

   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );
}
*/
