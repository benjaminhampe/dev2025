#include "PianoKeyboardCtrl_wx.hpp"

BEGIN_EVENT_TABLE(PianoKeyboardCtrl, wxControl)
   EVT_TIMER(TimerID_DrawPiano, PianoKeyboardCtrl::timerEvent)
   EVT_SIZE(PianoKeyboardCtrl::resizeEvent)
   EVT_PAINT(PianoKeyboardCtrl::paintEvent)
   EVT_ERASE_BACKGROUND(PianoKeyboardCtrl::backgroundEvent)
   EVT_MOUSE_EVENTS(PianoKeyboardCtrl::mouseEvent)
   EVT_KEY_DOWN(PianoKeyboardCtrl::keyPressEvent)
   EVT_KEY_UP(PianoKeyboardCtrl::keyReleaseEvent)
   EVT_ENTER_WINDOW(PianoKeyboardCtrl::enterEvent)
   EVT_LEAVE_WINDOW(PianoKeyboardCtrl::leaveEvent)
END_EVENT_TABLE()

PianoKeyboardCtrl::PianoKeyboardCtrl( wxWindow* parent, wxWindowID id, wxPoint const & pos, wxSize const & size, long style )
   : wxControl( parent, id, pos, size, style, wxDefaultValidator )
   , m_Timer( this, TimerID_DrawPiano )
   , m_IsEntered( false )
{
   //SetBackgroundStyle( wxBG_STYLE_CUSTOM );
   SetBackgroundStyle( wxBG_STYLE_PAINT );

   //RtMidiUtils::testOutput();
   m_Engine = new RtMidiEngine();
}

PianoKeyboardCtrl::~PianoKeyboardCtrl()
{
   delete m_Engine;
}


void
PianoKeyboardCtrl::paintEvent(wxPaintEvent& event)
{
   static_cast< void >( event );
   //wxAutoBufferedPaintDC dc( this );
   wxPaintDC dc( this );
   int w = dc.GetSize().x;
   int h = dc.GetSize().y;
   dc.SetPen( wxNullPen );
   dc.SetBrush( wxBrush( wxColour(55,168,85), wxBRUSHSTYLE_SOLID ) );
   dc.DrawRectangle( wxRect( 0,0,w,h) );

   dc.SetPen( wxPen( wxColour(55,55,55), 1 ) );
   dc.SetBrush( wxBrush( wxColour(25,25,25), wxBRUSHSTYLE_SOLID ) );
   int kw = w / 7;
   int kh = h / 2;
   int x = 0;
   int y = 0;
   for ( int i = 0; i < 7; ++i )
   {
      dc.DrawRoundedRectangle( wxRect( x,y,kw,h), 7.0 );
      x += kw;
   }

   dc.SetPen( wxPen( wxColour(55,55,55), 1 ) );
   dc.SetBrush( wxBrush( wxColour(255,255,255), wxBRUSHSTYLE_SOLID ) );
   x = kw/2;
   y = 0;
   for ( int i = 0; i < 2; ++i )
   {
      dc.DrawRoundedRectangle( wxRect( x,y,kw,kh), 7.0 );
      x += kw;
   }
   x += kw;
   for ( int i = 0; i < 3; ++i )
   {
      dc.DrawRoundedRectangle( wxRect( x,y,kw,kh), 7.0 );
      x += kw;
   }
}


void PianoKeyboardCtrl::timerEvent( wxTimerEvent& event)
{
   if ( event.GetTimer().GetId() == TimerID_DrawPiano )
   {
      Refresh( false ); // render();
      event.Skip();
   }
}

void PianoKeyboardCtrl::backgroundEvent( wxEraseEvent& event )
{
   Refresh( false );
   event.Skip();
}

void PianoKeyboardCtrl::resizeEvent( wxSizeEvent& event )
{
   Refresh( false );
   event.Skip();
}

void
PianoKeyboardCtrl::enterEvent( wxMouseEvent& event )
{
   m_IsEntered = true;
   Refresh( false );
   int mx = event.GetX();
   int my = event.GetY();
   DE_DEBUG("mousePos(",mx,",",my,")")
}
void
PianoKeyboardCtrl::leaveEvent( wxMouseEvent& event )
{
   m_IsEntered = false;
   Refresh( false );

   int mx = event.GetX();
   int my = event.GetY();
   DE_DEBUG("mousePos(",mx,",",my,")")
}


void
PianoKeyboardCtrl::mouseEvent(wxMouseEvent& event)
{
   int mx = event.GetX();
   int my = event.GetY();
   float wheel = (float)event.GetWheelRotation();

   m_MousePos = glm::ivec2( mx, my ); // current mouse pos

   if ( m_MousePos != m_MousePosLast ) // Some button is clicked...
   {
      m_MouseMove = m_MousePos - m_MousePosLast; // current mouse pos

      m_MousePosLast = m_MousePos;
   }
   else
   {
      m_MouseMove = { 0,0 };
   }

//   if (event.IsButton())
//   {
//      if (event.LeftDown()) m_IsLeftPressed = true;
//      else if (event.LeftUp()) m_IsLeftPressed = false;
//      else if (event.MiddleDown()) m_IsMiddlePressed = true;
//      else if (event.MiddleUp()) m_IsMiddlePressed = false;
//      else if (event.RightDown()) m_IsRightPressed = true;
//      else if (event.RightUp()) m_IsRightPressed = false;
//   }

   event.Skip();
}

void
PianoKeyboardCtrl::keyPressEvent(wxKeyEvent& event)
{
   int kc = event.GetKeyCode();

   uint8_t m_Channel = 0;
   uint8_t m_Octave = 5;
   uint8_t m_Velocity = 90;
   uint8_t k = 0;
   // Oktave on keyboard
   if ( kc == 'q' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == '2' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'w' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == '3' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'e' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'r' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == '5' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 't' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == '6' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   //if ( kc == 'y' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } // y and z get same key for querty and quertz keyboards
   if ( kc == 'z' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == '7' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'u' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

   // Oktave on keyboard
   m_Octave = 4;
   k = 0;
   if ( kc == '<' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'a' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'y' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 's' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'x' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

   if ( kc == 'c' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'f' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'v' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'g' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'b' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'h' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
   if ( kc == 'n' ) { m_Engine->noteOn( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

   event.Skip();
}

void
PianoKeyboardCtrl::keyReleaseEvent( wxKeyEvent& event )
{
   int kc = event.GetKeyCode();

//   uint8_t m_Channel = 0;
//   uint8_t m_Octave = 5;
//   uint8_t m_Velocity = 90;
//   uint8_t k = 0;
//   // Oktave on keyboard
//   if ( kc == 'q' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == '2' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'w' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == '3' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'e' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'r' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == '5' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 't' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == '6' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'z' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == '7' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'u' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

//   // Oktave on keyboard
//   m_Octave = 4;
//   k = 0;
//   if ( kc == '<' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'a' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'y' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 's' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'x' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

//   if ( kc == 'c' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'f' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'v' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'g' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'b' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'h' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;
//   if ( kc == 'n' ) { m_Engine->noteOff( k + 12*m_Octave, m_Velocity, m_Channel ); } k++;

   event.Skip();

}

