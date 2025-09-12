#include "GMidiMasterKeyboard.hpp"
#include <QPainter>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

namespace {

// =======================================================================
struct MyMidiTools
// =======================================================================
{
   //DE_CREATE_LOGGER("de.MidiTools")

   static bool
   isBlackPianoKey( int semi )
   {
      if ( semi == 1 // C = 0, C# = 1,
        || semi == 3 // D = 2, D# = 3, E = 4,
        || semi == 6 // F = 5, F# = 6,
        || semi == 8 // G = 7, G# = 8,
        || semi == 10)//A = 9, A# = 10, H = 11
      {
         return true; // black piano key
      }
      return false; // white piano key
   }
};

} // end namespace

// ====================================================================
KeyboardArea::KeyboardArea( QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_isRepeatingPressEvents( false )
   , m_timerId( 0 )
   , m_keyStart( 0 )
   , m_keyWidth( 1 )
   , m_whiteKeyCount( 1 )
   , m_blackKeyCount( 1 )
   , m_timeLastNote( 0.0 )
   , m_timeNote( 0.0 )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumSize( 120, 64 );
   setFocusPolicy( Qt::StrongFocus );
   setMouseTracking( true );
   for ( auto & synth : m_synths ) { synth = nullptr; }
   for ( auto & key : m_touched ) { key = nullptr; }
   setKeyRange( 24, 100 );

//   de::Image imgC = createImage( 0, 24, 64 );
//   dbSaveImage( imgC, "Audio_imgC.png");
//   QImage m_imgC = createKeyImage( 0, 24, 64 );
//   m_imgC.save("Audio_m_imgC.png");
}

KeyboardArea::~KeyboardArea()
{

}
void
KeyboardArea::setKeyRange( int midiNoteStart, int midiNoteCount )
{
   m_keyStart = midiNoteStart;
   m_keys.clear();

   for ( int i = 0; i < midiNoteCount; i++ )
   {
      int midiNote = m_keyStart + i;
      int oktave = midiNote / 12;
      int semitone = midiNote - 12 * oktave;
      m_keys.emplace_back( midiNote, oktave, semitone );
   }

   m_whiteKeyCount = 0;
   m_blackKeyCount = 0;

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key const & key = m_keys[ i ];

      if ( MyMidiTools::isBlackPianoKey( key.semitone ) )
      {
         m_blackKeyCount++;
      }
      else
      {
         m_whiteKeyCount++;
      }
   }

//   std::cout << "KeyboardArea::setKeyRange(int midiNoteStart = " << midiNoteStart << ", int midiNoteCount = " << midiNoteCount << ")" << std::endl;

//   for ( int i = 0; i < m_keys.size(); i++ )
//   {
//      auto const & key = m_keys[ i ];
//      int midiNote = m_keyStart + i;
//      int oktave = midiNote / 12;
//      int semitone = midiNote - 12 * oktave;
//      std::cout << "Key[" << i << "] midiNote = " << midiNote << ", oktave = " << oktave << ", semitone = " << semitone << ", isBlackKey = " << MyMidiTools::isBlackPianoKey(semitone) << std::endl;
//   }


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

         for ( auto & synth : m_synths )
         {
            //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )
            if ( synth ) synth->sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
         }

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

         for ( auto & synth : m_synths )
         {
            //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )
            if ( synth ) synth->sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
         }

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
      for ( auto & synth : m_synths )
      {
         if ( synth ) synth->sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
      }
      //DE_DEBUG("midiNote(",midiNote,", velocity(",velocity,"), m_isRepeatingPressEvents(",m_isRepeatingPressEvents,")" )
      update();
   }
   else
   {
      int keyIndex = findKey( midiNote );
      if ( keyIndex > -1 && m_keys[ keyIndex ].pressed )
      {
         m_keys[ keyIndex ].pressed = false;
         for ( auto & synth : m_synths )
         {
            if ( synth ) synth->sendNote( de::audio::Note( 0, midiNote, 0, velocity ) );
         }

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

   for ( auto & synth : m_synths )
   {
      //if ( synth ) synth->allNotesOff();
   }

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
   int x = pos.x()+1;
   int y = pos.y()+1;
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
KeyboardArea::resizeEvent( QResizeEvent* event )
{
   m_keyWidth = int( float( event->size().width() ) / float( m_whiteKeyCount ) );
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

   for ( int i = 0; i < m_keys.size(); ++i )
   {
      Key const & key = m_keys[ i ];

      if ( !MyMidiTools::isBlackPianoKey( key.semitone ) )
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

      if ( MyMidiTools::isBlackPianoKey( key.semitone ) )
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
PitchWheel::PitchWheel( QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
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

   QPainter dc(this);
   //dc.setRenderHint( QPainter::NonCosmeticDefaultPen );

   dc.fillRect( rect(), QColor( 210,210,210 ) );

   dc.fillRect( QRect( 5,5,w-10, h-10), QColor( 10,10,10 ) );
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
GMidiMasterKeyboard::GMidiMasterKeyboard( QWidget* parent )
// ====================================================================
   : QWidget( parent ) // , Qt::NoDropShadowWindowHint
   , m_pitchWheel( nullptr )
   , m_modWheel( nullptr )
   , m_keyboardArea( nullptr )
   , m_btnRepeat( nullptr )
{
   setContentsMargins( 0,0,0,0 );
   setMinimumHeight( 96 );
   setMouseTracking( true );

   m_pitchWheel = new PitchWheel( this );
   m_modWheel = new PitchWheel( this );
   m_keyboardArea = new KeyboardArea( this );
   m_btnLeave = createLeaveButton();
   m_btnRepeat = createRepeatButton();

   auto v = new QVBoxLayout();
   v -> setContentsMargins( 0,0,0,0 );
   v -> addWidget( m_btnLeave );
   v -> addWidget( m_btnRepeat );

   auto h = new QHBoxLayout();
   h -> setContentsMargins( 0,0,0,0 );
   h -> addWidget( m_pitchWheel );
   h -> addWidget( m_modWheel );
   h -> addWidget( m_keyboardArea,1 );
   h -> addLayout( v );
   setLayout( h );
}

GMidiMasterKeyboard::~GMidiMasterKeyboard()
{

}

void
GMidiMasterKeyboard::keyPressEvent( QKeyEvent* event )
{
   QWidget::keyPressEvent( event );
   /*
   auto noteOn = [&] ( int midiNote, int velocity = 90 )
   {
      if ( m_keyboardArea ) m_keyboardArea->noteOn( midiNote, velocity );
   };

   if ( !event->isAutoRepeat() )
   {
      auto key = event->key();

      // DE_DEBUG("KeyPress(",key,")")

      if ( key == Qt::Key_1 ) { noteOn( 59 ); }
      if ( key == Qt::Key_Q ) { noteOn( 60 ); }
      if ( key == Qt::Key_2 ) { noteOn( 61 ); }
      if ( key == Qt::Key_W ) { noteOn( 62 ); }
      if ( key == Qt::Key_3 ) { noteOn( 63 ); }
      if ( key == Qt::Key_E ) { noteOn( 64 ); }
      if ( key == Qt::Key_R ) { noteOn( 65 ); }
      if ( key == Qt::Key_5 ) { noteOn( 66 ); }
      if ( key == Qt::Key_T ) { noteOn( 67 ); }
      if ( key == Qt::Key_6 ) { noteOn( 68 ); }
      if ( key == Qt::Key_Z ) { noteOn( 69 ); }
      if ( key == Qt::Key_7 ) { noteOn( 70 ); }
      if ( key == Qt::Key_U ) { noteOn( 71 ); }
      if ( key == Qt::Key_I ) { noteOn( 72 ); }
      if ( key == Qt::Key_9 ) { noteOn( 73 ); }
      if ( key == Qt::Key_O ) { noteOn( 74 ); }
      if ( key == Qt::Key_0 ) { noteOn( 75 ); }
      if ( key == Qt::Key_P ) { noteOn( 76 ); }

      if ( key == Qt::Key_Greater )    { noteOn( 48 ); }
      if ( key == Qt::Key_Less )       { noteOn( 48 ); }
      if ( key == Qt::Key_A )          { noteOn( 49 ); }
      if ( key == Qt::Key_Y )          { noteOn( 50 ); }
      if ( key == Qt::Key_S )          { noteOn( 51 ); }
      if ( key == Qt::Key_X )          { noteOn( 52 ); }
      if ( key == Qt::Key_C )          { noteOn( 53 ); }
      if ( key == Qt::Key_F )          { noteOn( 54 ); }
      if ( key == Qt::Key_V )          { noteOn( 55 ); }
      if ( key == Qt::Key_G )          { noteOn( 56 ); }
      if ( key == Qt::Key_B )          { noteOn( 57 ); }
      if ( key == Qt::Key_H )          { noteOn( 58 ); }
      if ( key == Qt::Key_N )          { noteOn( 59 ); }
      if ( key == Qt::Key_M )          { noteOn( 60 ); }
      if ( key == Qt::Key_K )          { noteOn( 61 ); }
      if ( key == Qt::Key_Comma )      { noteOn( 62 ); }
      if ( key == Qt::Key_L )          { noteOn( 63 ); }
      if ( key == Qt::Key_Period )     { noteOn( 64 ); }
      if ( key == Qt::Key_Minus )      { noteOn( 65 ); } // F
      if ( key == Qt::Key_Odiaeresis ) { noteOn( 66 ); } // Ö = F#
      if ( key == Qt::Key_Adiaeresis ) { noteOn( 66 ); } // Ä = F#
      if ( key == Qt::Key_NumberSign ) { noteOn( 68 ); } // # = G#

   }

   event->accept();
   */
}

void
GMidiMasterKeyboard::keyReleaseEvent( QKeyEvent* event )
{
   QWidget::keyReleaseEvent( event );
   /*
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

      if ( key == Qt::Key_1 ) { noteOff( 59 ); }
      if ( key == Qt::Key_Q ) { noteOff( 60 ); }
      if ( key == Qt::Key_2 ) { noteOff( 61 ); }
      if ( key == Qt::Key_W ) { noteOff( 62 ); }
      if ( key == Qt::Key_3 ) { noteOff( 63 ); }
      if ( key == Qt::Key_E ) { noteOff( 64 ); }
      if ( key == Qt::Key_R ) { noteOff( 65 ); }
      if ( key == Qt::Key_5 ) { noteOff( 66 ); }
      if ( key == Qt::Key_T ) { noteOff( 67 ); }
      if ( key == Qt::Key_6 ) { noteOff( 68 ); }
      if ( key == Qt::Key_Z ) { noteOff( 69 ); }
      if ( key == Qt::Key_7 ) { noteOff( 70 ); }
      if ( key == Qt::Key_U ) { noteOff( 71 ); }
      if ( key == Qt::Key_I ) { noteOff( 72 ); }
      if ( key == Qt::Key_9 ) { noteOff( 73 ); }
      if ( key == Qt::Key_O ) { noteOff( 74 ); }
      if ( key == Qt::Key_0 ) { noteOff( 75 ); }
      if ( key == Qt::Key_P ) { noteOff( 76 ); }

      if ( key == Qt::Key_Greater )    { noteOff( 48 ); }
      if ( key == Qt::Key_Less )       { noteOff( 48 ); }
      if ( key == Qt::Key_A )          { noteOff( 49 ); }
      if ( key == Qt::Key_Y )          { noteOff( 50 ); }
      if ( key == Qt::Key_S )          { noteOff( 51 ); }
      if ( key == Qt::Key_X )          { noteOff( 52 ); }
      if ( key == Qt::Key_C )          { noteOff( 53 ); }
      if ( key == Qt::Key_F )          { noteOff( 54 ); }
      if ( key == Qt::Key_V )          { noteOff( 55 ); }
      if ( key == Qt::Key_G )          { noteOff( 56 ); }
      if ( key == Qt::Key_B )          { noteOff( 57 ); }
      if ( key == Qt::Key_H )          { noteOff( 58 ); }
      if ( key == Qt::Key_N )          { noteOff( 59 ); }
      if ( key == Qt::Key_M )          { noteOff( 60 ); }
      if ( key == Qt::Key_K )          { noteOff( 61 ); }
      if ( key == Qt::Key_Comma )      { noteOff( 62 ); }
      if ( key == Qt::Key_L )          { noteOff( 63 ); }
      if ( key == Qt::Key_Period )     { noteOff( 64 ); }
      if ( key == Qt::Key_Minus )      { noteOff( 65 ); } // F
      if ( key == Qt::Key_Odiaeresis ) { noteOff( 66 ); } // Ö = F#
      if ( key == Qt::Key_Adiaeresis ) { noteOff( 66 ); } // Ä = F#
      if ( key == Qt::Key_NumberSign ) { noteOff( 68 ); } // # = G#
   }
   event->accept();
   */
}

QImageButton*
GMidiMasterKeyboard::createLeaveButton()
{
   auto btn = new QImageButton( this );
   int bw = 48;
   int bh = 17;
   btn->setMinimumSize( bw,bh );
//   QFont font = getFontAwesome();
//   QString msg = "Repeat\nOff";

//   // [idle]
//   QImage ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.headerBtnFillColor );
//   QImage img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.headerBtnFillColor, ico );
//   btn->setImage( 0, img );
//   // [idle_hover]
//   btn->setImage( 1, img );

//   // [active]
//   msg = "Repeat\nOn";
//   ico = createImageFromText( 1,1, msg, font, skin.headerBtnTextColor, skin.activeColor );
//   img = createRectImage( bw,bh, skin.headerBtnPenColor, skin.activeColor, ico );
//   btn->setImage( 2, img );
//   // [active_hover]
//   btn->setImage( 3, img );
   return btn;
}


QImageButton*
GMidiMasterKeyboard::createRepeatButton()
{
   auto btn = new QImageButton( this );
   int bw = 48;
   int bh = 17;
   btn->setMinimumSize( bw,bh );
   /*
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
   */
   return btn;
}
