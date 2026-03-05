#include "MidiMasterKeyboard.h"
#include <QPainter>
#include <QDebug>
#include <de/midi/MidiTools.h>
#include "App.h"

// ====================================================================
PitchWheel::PitchWheel( App & app, QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_app( app ) // FOR SKIN MAYBE
   , m_sliderValue( 0 )
   , m_controlValue( 0 )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumWidth( 32 );
   setMouseTracking( true );
   //setFocusPolicy( Qt::StrongFocus );
}

PitchWheel::~PitchWheel()
{

}

void
PitchWheel::paintEvent( QPaintEvent* event )
{
   int w = width();
   int h = height();

   QColor fillColor(20,20,20);
   QColor penColor(80,80,80);

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );


   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( fillColor ) );
   dc.drawRect( QRect(1,1,w-2,h-2) );

   dc.setBrush( Qt::NoBrush );
   dc.setPen( QPen( penColor ) );
   dc.drawLine( 1,0, w-2,0 );
   dc.drawLine( 0,h-1, w-2,h-1 );
   dc.drawLine( 0,1, 0,h-2 );
   dc.drawLine( w-1,1, w-1,h-2 );

//   dc
//   dc.fillRect( rect(), QColor( 210,210,210 ) );

//   dc.fillRect( QRect( 5,5,w-10, h-10), QColor( 10,10,10 ) );
}

void
PitchWheel::resizeEvent( QResizeEvent* event )
{
   update();
   QWidget::resizeEvent( event );
}

void
PitchWheel::mouseMoveEvent( QMouseEvent* event )
{
   // m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   //event->accept();
   QWidget::mouseMoveEvent( event );
}

void
PitchWheel::mousePressEvent( QMouseEvent* event )
{
   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   //event->accept();
   QWidget::mousePressEvent( event );

}

void
PitchWheel::mouseReleaseEvent( QMouseEvent* event )
{
   //event->accept();
   QWidget::mouseReleaseEvent( event );

}

void
PitchWheel::wheelEvent( QWheelEvent* event )
{
   //event->accept();
   QWidget::wheelEvent( event );
}


// ====================================================================
KeyboardArea::KeyboardArea( App & app, QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_app( app )
   , m_isRepeatingPressEvents( false )
   , m_timerId( 0 )
   , m_midiNoteStart( 0 )
   , m_midiNoteCount( 0 )
   , m_keyWidth( 1 )
   , m_midiNoteCountWhite( 1 )
   , m_midiNoteCountBlack( 1 )
   , m_timeLastNote( 0.0 )
   , m_timeNote( 0.0 )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 120, 64 );
   setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );
   for ( auto & synth : m_synths ) { synth = nullptr; }
   for ( auto & key : m_touched ) { key = nullptr; }
   setKeyRange( 24, 60 );
}

KeyboardArea::~KeyboardArea()
{

}
void
KeyboardArea::setKeyRange( int midiNoteStart, int midiNoteCount )
{
   m_midiNoteStart = midiNoteStart;
   m_midiNoteCount = midiNoteCount;
   m_keys.clear();

   for ( int i = 0; i < midiNoteCount; i++ )
   {
      Key key;
      key.midiNote = m_midiNoteStart + i;
      key.oktave = key.midiNote / 12;
      key.semitone = key.midiNote - 12 * key.oktave;
      key.isBlackKey = de::midi::MidiTools::isBlackPianoKey( key.semitone );
      m_keys.emplace_back( std::move( key ) );
   }

   m_midiNoteCountWhite = 0;
   m_midiNoteCountBlack = 0;

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key const & key = m_keys[ i ];

      if ( de::midi::MidiTools::isBlackPianoKey( key.semitone ) )
      {
         m_midiNoteCountBlack++;
      }
      else
      {
         m_midiNoteCountWhite++;
      }
   }

    DE_INFO(m_midiNoteStart,",",m_midiNoteCount)
}

int32_t
KeyboardArea::findKey( int midiNote ) const
{
   auto it = std::find_if( m_keys.begin(), m_keys.end(), [&] (Key const & key) { return key.midiNote == midiNote; });
   if ( it != m_keys.end() )
   {
      return int32_t( std::distance( m_keys.begin(), it ) );
   }
   else
   {
      return -1;
   }
}

void
KeyboardArea::noteOn( int midiNote, int velocity )
{
   if ( m_isRepeatingPressEvents ) // Old
   {
      int keyIndex = findKey( midiNote );
      if ( keyIndex > -1 )
      {
         m_keys[ keyIndex ].pressed = true;

         m_app.sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
         //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )

         update();
      }

      //      int keyIndex = findKey( midiNote );
      //      if ( keyIndex > -1 )
      //      {
      //         m_keys[ keyIndex ].pressed = true;
      //      }
   }
   else
   {
      int keyIndex = findKey( midiNote );
      if ( keyIndex > -1 && !m_keys[ keyIndex ].pressed )
      {
         m_keys[ keyIndex ].pressed = true;
         m_app.sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
         //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )

         update();
      }
   }
}

void
KeyboardArea::noteOff( int midiNote )
{
   int velocity = 0;
   if ( m_isRepeatingPressEvents ) // Old
   {
      //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )
      m_app.sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
      update();
   }
   else
   {
      int keyIndex = findKey( midiNote );
      if ( keyIndex > -1 && m_keys[ keyIndex ].pressed )
      {
         m_keys[ keyIndex ].pressed = false;
         m_app.sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );

         //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )
         update();
      }
   }

   /*
   int keyIndex = findKey( midiNote );
   if ( keyIndex > -1 )
   {
      m_keys[ keyIndex ].pressed = false;
   }
   */
}

void
KeyboardArea::enterEvent( QEnterEvent* event )
{
   //m_isHovered = true;
   update();
   QWidget::enterEvent( event );
}
void
KeyboardArea::leaveEvent( QEvent* event )
{
   //m_isHovered = false;


   m_app.allNotesOff();

   update();


   QWidget::leaveEvent( event );
}

/*
void
KeyboardArea::focusInEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusInEvent( event );
}

void
KeyboardArea::focusOutEvent( QFocusEvent* event )
{
   m_hasFocus = true;
   update();
   QWidget::focusOutEvent( event );
}
*/

namespace {

void drawKeyH( QPainter & dc, QRect pos, QColor brushColor, QColor penColor )
{
   int x = pos.x();
   int y = pos.y();
   int w = pos.width()-1;
   int h = pos.height()-2;
   dc.setPen( Qt::NoPen );
   dc.setBrush( QBrush( brushColor ) );
   dc.drawRect( QRect(x+1,y+1,w-2,h-2) );

   dc.setPen( QPen( penColor ) );
   dc.drawLine( x+1,y, x+w-2, y );
   dc.drawLine( x+1,y+h-1, x+w-2, y+h-1 );
   dc.drawLine( x,y+1, x, y+h-2 );
   dc.drawLine( x+w-1,y+1, x+w-1, y+h-2 );
}

} // end namespace

int
KeyboardArea::keyIndexFromMouse( int mx, int my ) const
{
   return -1;
}


void
KeyboardArea::updateKeyboardLayout()
{
   if ( m_keys.empty() ) return; // prevent div by zero

   int w = (width()-int(m_keys.size())) / int(m_keys.size());
   int h = height();
   int x = 0;
   int y = 0;

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key & key = m_keys[ i ];
      key.rect = QRect(x,y,w,h);
      x += w+1;
   }
}

void
KeyboardArea::resizeEvent( QResizeEvent* event )
{
   m_keyWidth = int( float( event->size().width() ) / float( m_midiNoteCountWhite ) );
   update();
}

void
KeyboardArea::paintEvent( QPaintEvent* event )
{
   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );
   //dc.fillRect( rect(), QColor( 10,10,10 ) );

   int w = width();
   int h = height();

   QColor whiteBrushColor(255,255,255);
   QColor whitePenColor(205,205,205);
   QColor blackBrushColor(2,2,2);
   QColor blackPenColor(65,65,65);

   // Count white keys
   int key_w = m_keyWidth;
   int key_h = h;
   int x = 0;
   int y = 0;

   //m_keys[ keyIndex ].pressed = true;
   //int keyCount = std::min( int(m_keys.size()), k );
   //int whiteCount = std::min( int(m_keys.size()), k );
   dc.setPen( QPen( QColor(200,100,100) ) );
   dc.setBrush( Qt::NoBrush );
   dc.drawLine( 0,0, w-1,0 );
   //dc.drawRect( 0,0, w, 1 );

   updateKeyboardLayout();

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key const & key = m_keys[ i ];
      if ( !de::midi::MidiTools::isBlackPianoKey( key.semitone ) )
      {
         if ( key.pressed )
         {
            drawKeyH( dc, QRect(x,y,key_w,key_h), QColor( 55,55,255 ), QColor( 155,155,255 ) );
         }
         else
         {
            drawKeyH( dc, QRect(x,y,key_w,key_h), whiteBrushColor, whitePenColor );
         }
         x += key_w;
      }
   }
   drawKeyH( dc, QRect(x,y,key_w,key_h), whiteBrushColor, whitePenColor );

   x = key_w / 2;
   y = 0;
   int bkw = key_w;
   int bkh = key_h / 2;

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key const & key = m_keys[ i ];

      if ( de::midi::MidiTools::isBlackPianoKey( key.semitone ) )
      {
         if ( key.pressed )
         {
            drawKeyH( dc, QRect(x,y,bkw,bkh), QColor( 255,255,105 ), QColor( 255,255,200 ) );
         }
         else
         {
            drawKeyH( dc, QRect(x,y,bkw,bkh), blackBrushColor, blackPenColor );
         }

         if ( key.semitone == 3 )
         {
            x += key_w;
         }

         if ( key.semitone == 10 )
         {
            x += key_w;
         }

         x += key_w;
      }
   }

//   m_font5x8.drawText( dc, w/2, 1, QString("L_min(%1), L_max(%2), R_min(%3), R_max(%4)")
//      .arg( L_min).arg( L_max ).arg( R_min).arg( R_max ), 0xFFFFFFFF, de::Align::CenterTop );

}


void
KeyboardArea::timerEvent( QTimerEvent* event )
{
   if ( event->timerId() == m_timerId )
   {
      update();
   }
}


void
KeyboardArea::mouseMoveEvent( QMouseEvent* event )
{

   // m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   // if ( m_MousePos != m_LastMousePos ) // Some button is clicked...
   // {
      // m_MouseMove = m_MousePos - m_LastMousePos; // current mouse pos
      // m_LastMousePos = m_MousePos;
   // }
   // else
   // {
      // m_MouseMove = { 0,0 };
   // }
}

void
KeyboardArea::mousePressEvent( QMouseEvent* event )
{
   // if ( m_Driver )
   // {
      // de::SEvent post;
      // post.type = de::EET_MOUSE_EVENT;
      // post.mouseEvent.m_Flags = de::SMouseEvent::Pressed;
      // post.mouseEvent.m_X = event->x();
      // post.mouseEvent.m_Y = event->y();
      // post.mouseEvent.m_Wheel = 0.0f;

      // if ( event->button() == Qt::LeftButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnLeft;
      // }
      // else if ( event->button() == Qt::RightButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnRight;
      // }
      // else if ( event->button() == Qt::MiddleButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnMiddle;
      // }
      // m_Driver->postEvent( post );

      // auto camera = m_Driver->getActiveCamera();
      // if ( camera )
      // {
         // de::gpu::Ray3d ray = camera->computeRay( event->x(), event->y() );
         // auto a = ray.getPos() + ray.getDir() * 10.0;
         // auto b = ray.getPos() + ray.getDir() * 1000.0;
         // m_LineRenderer.add3DLine( a,b, de::randomColor() );
      // }

   // }

   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
}

void
KeyboardArea::mouseReleaseEvent( QMouseEvent* event )
{
   // if ( m_Driver )
   // {
      // de::SEvent post;
      // post.type = de::EET_MOUSE_EVENT;
      // post.mouseEvent.m_Flags = de::SMouseEvent::Released;
      // post.mouseEvent.m_X = event->x();
      // post.mouseEvent.m_Y = event->y();
      // post.mouseEvent.m_Wheel = 0.0f;
      // if ( event->button() == Qt::LeftButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnLeft;
      // }
      // else if ( event->button() == Qt::RightButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnRight;
      // }
      // else if ( event->button() == Qt::MiddleButton )
      // {
         // post.mouseEvent.m_Flags |= de::SMouseEvent::BtnMiddle;
      // }
      // m_Driver->postEvent( post );
   // }

   //m_MousePos = glm::ivec2( event->x(), event->y() ); // current mouse pos
   //m_MouseMove = { 0,0 };
}


void
KeyboardArea::wheelEvent( QWheelEvent* event )
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


// ====================================================================
MidiMasterKeyboard::MidiMasterKeyboard( App & app, QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_app( app )
   , m_pitchWheel( nullptr )
   , m_modWheel( nullptr )
   , m_keyboardArea( nullptr )
   , m_btnRepeat( nullptr )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumHeight( 96 );
   setMouseTracking( true );

   m_pitchWheel = new PitchWheel( m_app, this );
   m_modWheel = new PitchWheel( m_app, this );
   m_keyboardArea = new KeyboardArea( m_app, this );
   m_btnNotesOff = new QPushButton( "SendNotesOff", this );
   m_btnAftertouch = new QPushButton( "Aftertouch", this );
   m_edtMidiStart = new QDial( this );
   m_edtMidiCount = new QDial( this );
   m_btnLeave = createLeaveButton();
   m_btnRepeat = createRepeatButton();

   auto h1 = new QHBoxLayout();
   h1 -> setContentsMargins( 0,0,0,0 );
   h1 -> addWidget( m_btnNotesOff );
   h1 -> addWidget( m_btnAftertouch );

   auto h2 = new QHBoxLayout();
   h2 -> setContentsMargins( 0,0,0,0 );
   h2 -> addWidget( m_edtMidiStart );
   h2 -> addWidget( m_edtMidiCount );

   auto h3 = new QHBoxLayout();
   h3 -> setContentsMargins( 0,0,0,0 );
   h3 -> addWidget( m_btnLeave );
   h3 -> addWidget( m_btnRepeat );

   auto v = new QVBoxLayout();
   v -> setContentsMargins( 0,0,0,0 );
   v -> addLayout( h1 );
   v -> addLayout( h2 );
   v -> addLayout( h3 );

   auto h = new QHBoxLayout();
   h -> setContentsMargins( 0,0,0,0 );
   h -> addWidget( m_pitchWheel );
   h -> addWidget( m_modWheel );
   h -> addWidget( m_keyboardArea,1 );
   h -> addLayout( v );
   setLayout( h );


}

MidiMasterKeyboard::~MidiMasterKeyboard()
{

}

void
MidiMasterKeyboard::keyPressEvent( QKeyEvent* event )
{
   auto noteOn = [&] ( int midiNote, int velocity = 90 )
   {
      if ( m_keyboardArea ) m_keyboardArea->noteOn( midiNote, velocity );
   };

   if ( !event->isAutoRepeat() )
   {
      auto key = event->key();

      int k = 12+59;
      if ( key == Qt::Key_1 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Q )          { noteOn( k ); } k++;
      if ( key == Qt::Key_2 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_W )          { noteOn( k ); } k++;
      if ( key == Qt::Key_3 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_E )          { noteOn( k ); } k++;
      if ( key == Qt::Key_R )          { noteOn( k ); } k++;
      if ( key == Qt::Key_5 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_T )          { noteOn( k ); } k++;
      if ( key == Qt::Key_6 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Z )          { noteOn( k ); } k++;
      if ( key == Qt::Key_7 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_U )          { noteOn( k ); } k++;
      if ( key == Qt::Key_I )          { noteOn( k ); } k++;
      if ( key == Qt::Key_9 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_O )          { noteOn( k ); } k++;
      if ( key == Qt::Key_0 )          { noteOn( k ); } k++;
      if ( key == Qt::Key_P )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Udiaeresis ) { noteOn( k ); } k++;
      if ( key == Qt::Key_ssharp )     { noteOn( k ); } k++;
      if ( key == Qt::Key_acute )      { noteOn( k ); } k++;
      if ( key == Qt::Key_Plus )       { noteOn( k ); } k++;

      k = 48;
      if ( key == Qt::Key_Greater )    { noteOn( k ); }
      if ( key == Qt::Key_Less )       { noteOn( k ); } k++;
      if ( key == Qt::Key_A )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Y )          { noteOn( k ); } k++;
      if ( key == Qt::Key_S )          { noteOn( k ); } k++;
      if ( key == Qt::Key_X )          { noteOn( k ); } k++;
      if ( key == Qt::Key_C )          { noteOn( k ); } k++;
      if ( key == Qt::Key_F )          { noteOn( k ); } k++;
      if ( key == Qt::Key_V )          { noteOn( k ); } k++;
      if ( key == Qt::Key_G )          { noteOn( k ); } k++;
      if ( key == Qt::Key_B )          { noteOn( k ); } k++;
      if ( key == Qt::Key_H )          { noteOn( k ); } k++;
      if ( key == Qt::Key_N )          { noteOn( k ); } k++;
      if ( key == Qt::Key_M )          { noteOn( k ); } k++;
      if ( key == Qt::Key_K )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Comma )      { noteOn( k ); } k++;
      if ( key == Qt::Key_L )          { noteOn( k ); } k++;
      if ( key == Qt::Key_Period )     { noteOn( k ); } k++;
      if ( key == Qt::Key_Minus )      { noteOn( k ); } k++; // F
      if ( key == Qt::Key_Odiaeresis ) { noteOn( k ); } k++; // Ö = F#
      if ( key == Qt::Key_Adiaeresis ) { noteOn( k ); } k++; // Ä = F#
      if ( key == Qt::Key_NumberSign ) { noteOn( k ); } k++; // # = G#

   }

   event->accept();
}

void
MidiMasterKeyboard::keyReleaseEvent( QKeyEvent* event )
{
   auto noteOff = [&] ( int midiNote )
   {
      if ( m_keyboardArea )
      {
         m_keyboardArea->noteOff( midiNote );
      }
   };

   if ( !event->isAutoRepeat() )
   {
      auto key = event->key();

      // DE_DEBUG("keyRelease(",key,")")

      int k = 12+59;
      if ( key == Qt::Key_1 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Q )          { noteOff( k ); } k++;
      if ( key == Qt::Key_2 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_W )          { noteOff( k ); } k++;
      if ( key == Qt::Key_3 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_E )          { noteOff( k ); } k++;
      if ( key == Qt::Key_R )          { noteOff( k ); } k++;
      if ( key == Qt::Key_5 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_T )          { noteOff( k ); } k++;
      if ( key == Qt::Key_6 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Z )          { noteOff( k ); } k++;
      if ( key == Qt::Key_7 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_U )          { noteOff( k ); } k++;
      if ( key == Qt::Key_I )          { noteOff( k ); } k++;
      if ( key == Qt::Key_9 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_O )          { noteOff( k ); } k++;
      if ( key == Qt::Key_0 )          { noteOff( k ); } k++;
      if ( key == Qt::Key_P )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Udiaeresis ) { noteOff( k ); } k++;
      if ( key == Qt::Key_ssharp )     { noteOff( k ); } k++;
      if ( key == Qt::Key_acute )      { noteOff( k ); } k++;
      if ( key == Qt::Key_Plus )       { noteOff( k ); } k++;

      k = 48;
      if ( key == Qt::Key_Greater )    { noteOff( k ); }
      if ( key == Qt::Key_Less )       { noteOff( k ); } k++;
      if ( key == Qt::Key_A )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Y )          { noteOff( k ); } k++;
      if ( key == Qt::Key_S )          { noteOff( k ); } k++;
      if ( key == Qt::Key_X )          { noteOff( k ); } k++;
      if ( key == Qt::Key_C )          { noteOff( k ); } k++;
      if ( key == Qt::Key_F )          { noteOff( k ); } k++;
      if ( key == Qt::Key_V )          { noteOff( k ); } k++;
      if ( key == Qt::Key_G )          { noteOff( k ); } k++;
      if ( key == Qt::Key_B )          { noteOff( k ); } k++;
      if ( key == Qt::Key_H )          { noteOff( k ); } k++;
      if ( key == Qt::Key_N )          { noteOff( k ); } k++;
      if ( key == Qt::Key_M )          { noteOff( k ); } k++;
      if ( key == Qt::Key_K )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Comma )      { noteOff( k ); } k++;
      if ( key == Qt::Key_L )          { noteOff( k ); } k++;
      if ( key == Qt::Key_Period )     { noteOff( k ); } k++;
      if ( key == Qt::Key_Minus )      { noteOff( k ); } k++; // F
      if ( key == Qt::Key_Odiaeresis ) { noteOff( k ); } k++; // Ö = F#
      if ( key == Qt::Key_Adiaeresis ) { noteOff( k ); } k++; k++; // Ä = F#
      if ( key == Qt::Key_NumberSign ) { noteOff( k ); } k++; // # = G#
   }
   event->accept();
}

ImageButton*
MidiMasterKeyboard::createLeaveButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto btn = new ImageButton( this );

   int bw = 48;
   int bh = 17;

//   btn->setCheckable( true );
//   btn->setChecked( false );

   QFont font = getFontAwesome();
   QString msg = "Repeat\nOff";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "Repeat\nOn";
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}


ImageButton*
MidiMasterKeyboard::createRepeatButton()
{
   LiveSkin const & skin = m_app.m_skin;
   auto btn = new ImageButton( this );

   int bw = 48;
   int bh = 17;

//   btn->setCheckable( true );
//   btn->setChecked( false );

   QFont font = getFontAwesome();
   QString msg = "Repeat\nOff";

   // [idle]
   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
   btn->setImage( 0, img );
   // [idle_hover]
   btn->setImage( 1, img );

   // [active]
   msg = "Repeat\nOn";
   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
   btn->setImage( 2, img );
   // [active_hover]
   btn->setImage( 3, img );
   return btn;
}

