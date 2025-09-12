#include "GMidiSequencer.hpp"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>

GMidiSequencer::GMidiSequencer( QWidget* parent )
   : QWidget( parent ), m_timerId( 0 )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 120, 64 );
   setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );
   setVisible(false );
   for ( auto & synth : m_synths ) { synth = nullptr; }
}

GMidiSequencer::~GMidiSequencer()
{

}

void
GMidiSequencer::setSynth( int i, de::audio::IDspChainElement* synth )
{
   if ( i < 0 || i >= m_synths.size() ) return;
   m_synths[ i ] = synth;
}

void
GMidiSequencer::paintEvent( QPaintEvent* event )
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   dc.fillRect( rect(), QColor( 100,100,200 ) );

//   int w = width();
//   int h = height();

//   dc.setCompositionMode( QPainter::CompositionMode_SourceOver );

//   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
//      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );

}


/*
void
GMidiSequencer::setKeyRange( int midiNoteStart, int midiNoteCount )
{
   m_noteStart = midiNoteStart;
   m_noteCount = midiNoteCount;

   m_keys.clear();

   for ( int i = 0; i < m_noteCount; i++ )
   {
      int midiNote = m_noteStart + i;
      int oktave = midiNote / 12;
      int semitone = midiNote - 12 * oktave;
      m_keys.emplace_back( midiNote, oktave, semitone );
   }
}

void
GMidiSequencer::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_timerId )
   {
      update();
   }
}


void
GMidiSequencer::resizeEvent( QResizeEvent* event )
{
   update();
}




void
GMidiSequencer::mouseMoveEvent( QMouseEvent* event )
{

}

void
GMidiSequencer::mousePressEvent( QMouseEvent* event )
{

}

void
GMidiSequencer::mouseReleaseEvent( QMouseEvent* event )
{
   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   //m_MouseMove = { 0,0 };
}


void
GMidiSequencer::wheelEvent( QWheelEvent* event )
{
   // if ( m_Driver )
   // {
      // de::SEvent post;
      // post.type = de::EET_MOUSE_EVENT;
      // post.mouseEvent.m_Flags = de::SMouseEvent::Wheel;
      // post.mouseEvent.m_X = event->x();
      // post.mouseEvent.m_Y = event->y();
      // post.mouseEvent.m_Wheel = event->angleDelta().y(); //    //QPoint delta = event->pixelDelta();
      // // DE_DEBUG("post.mouseEvent.m_Wheel = ",post.mouseEvent.m_Wheel)
      // m_Driver->postEvent( post );
   // }

   //event->accept();
}

void
GMidiSequencer::keyPressEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyPress(",event->key(),")")

   // if ( m_Driver )
   // {
      // de::SEvent post;
      // post.type = de::EET_KEY_EVENT;
      // post.keyEvent.Key = de::KEY_UNKNOWN;
      // post.keyEvent.Flags = de::SKeyEvent::Pressed;
      // if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
      // if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
      // if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
      // parseQtKey( event, post );
      // m_Driver->postEvent( post );
   // }

   auto noteOn = [&] ( int midiNote, int velocity = 90 )
   {
      for ( auto synth : m_synths )
      {
         if ( synth )
         {
            synth->sendNote( de::audio::synth::Note( -1, midiNote, 0, velocity ) );
         }
      }
   };

   auto key = event->key();

   int k = 59; // 59;
   if ( key == Qt::Key_1 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Q ) { noteOn( k ); } k++;
   if ( key == Qt::Key_2 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_W ) { noteOn( k ); } k++;
   if ( key == Qt::Key_3 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_E ) { noteOn( k ); } k++;
   if ( key == Qt::Key_R ) { noteOn( k ); } k++;
   if ( key == Qt::Key_5 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_T ) { noteOn( k ); } k++;
   if ( key == Qt::Key_6 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Z ) { noteOn( k ); } k++;
   if ( key == Qt::Key_7 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_U ) { noteOn( k ); } k++;
   if ( key == Qt::Key_I ) { noteOn( k ); } k++;
   if ( key == Qt::Key_9 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_O ) { noteOn( k ); } k++;
   if ( key == Qt::Key_0 ) { noteOn( k ); } k++;
   if ( key == Qt::Key_P ) { noteOn( k ); } k++;

   k = 48;
   if ( key == Qt::Key_Greater || key == Qt::Key_Less ) { noteOn( k ); } k++;
   if ( key == Qt::Key_A ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Y ) { noteOn( k ); } k++;
   if ( key == Qt::Key_S ) { noteOn( k ); } k++;
   if ( key == Qt::Key_X ) { noteOn( k ); } k++;
   //if ( key == Qt::Key_D ) { noteOn( k ); } k++;
   if ( key == Qt::Key_C ) { noteOn( k ); } k++;
   if ( key == Qt::Key_F ) { noteOn( k ); } k++;
   if ( key == Qt::Key_V ) { noteOn( k ); } k++;
   if ( key == Qt::Key_G ) { noteOn( k ); } k++;
   if ( key == Qt::Key_B ) { noteOn( k ); } k++;
   if ( key == Qt::Key_H ) { noteOn( k ); } k++;
   if ( key == Qt::Key_N ) { noteOn( k ); } k++;
   if ( key == Qt::Key_M ) { noteOn( k ); } k++;
   if ( key == Qt::Key_K ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Comma ) { noteOn( k ); } k++;
   if ( key == Qt::Key_L ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Period ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Odiaeresis ) { noteOn( k ); } k++; // Ö
   if ( key == Qt::Key_Minus ) { noteOn( k ); } k++;
   if ( key == Qt::Key_Adiaeresis ) { noteOn( k ); } k++; // Ä
   if ( key == Qt::Key_NumberSign ) { noteOn( k ); } k++; // #

   event->accept();
}

void
GMidiSequencer::keyReleaseEvent( QKeyEvent* event )
{
   //DE_DEBUG("KeyRelease(",event->key(),")")

   // if ( m_Driver )
   // {
      // de::SEvent post;
      // post.type = de::EET_KEY_EVENT;
      // post.keyEvent.Key = de::KEY_UNKNOWN;
      // post.keyEvent.Flags = de::SKeyEvent::None;
      // if ( event->modifiers() & Qt::ShiftModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Shift; }
      // if ( event->modifiers() & Qt::ControlModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Ctrl; }
      // if ( event->modifiers() & Qt::AltModifier ) { post.keyEvent.Flags |= de::SKeyEvent::Alt; }
      // parseQtKey( event, post );
      // m_Driver->postEvent( post );
   // }

   event->accept();
}


*/
