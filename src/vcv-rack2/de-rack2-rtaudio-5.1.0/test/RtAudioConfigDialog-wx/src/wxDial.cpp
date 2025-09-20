#include "wxDial.hpp"

BEGIN_EVENT_TABLE( wxDial, wxControl )
   EVT_TIMER(wxID_ANY, wxDial::timerEvent )
   //EVT_TIMER(BENNI_ID_FPS_TIMER, wxDial::timerEvent)
   EVT_SIZE(wxDial::resizeEvent)
   EVT_PAINT(wxDial::paintEvent)
   //EVT_ERASE_BACKGROUND( wxDial::backgroundEvent )
   EVT_MOUSE_EVENTS(wxDial::mouseEvent)
   EVT_KEY_DOWN(wxDial::keyPressEvent)
   EVT_KEY_UP(wxDial::keyReleaseEvent)
//   EVT_SET_FOCUS(wxDial::enterEvent)
//   EVT_KILL_FOCUS(wxDial::leaveEvent)
   EVT_ENTER_WINDOW(wxDial::enterEvent)
   EVT_LEAVE_WINDOW(wxDial::leaveEvent)

END_EVENT_TABLE()

wxDial::wxDial(
      wxWindow* parent,
      wxWindowID id,
      wxString const & title,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxControl( parent, id, pos, size, style, wxDefaultValidator )
   , m_Title( title )
   , m_Value( 75.0f )
   , m_Min( 0.0f )
   , m_Max( 100.0f )
   , m_Steps( 1024 )
   , m_MousePosX( 0 )
   , m_MousePosY( 0 )
   , m_LastMousePosX( 0 )
   , m_LastMousePosY( 0 )
   , m_MouseDragBegPosX( 0 )
   , m_MouseDragBegPosY( 0 )
   , m_MouseDragEndPosX( 0 )
   , m_MouseDragEndPosY( 0 )
   , m_IsDragging( false )
   , m_Timer( this )
{
   SetMinSize( wxSize(32,64) );
   Fit();
   //SetDoubleBuffered( true );
   SetBackgroundStyle( wxBG_STYLE_PAINT );
   // m_X->Bind( wxEVT_COMMAND_SPINCTRL_UPDATED, &wxDial::OnXChanged, this );
   //Connect( wxEVT_TIMER, wxTimerEventHandler(wxDial::timerEvent) );
   m_Timer.Bind( wxEVT_TIMER, wxDial::timerEvent, this );
   //setImage( createWxImageFromColor( 16,16, 0xFF1080F0 ) );

}

wxDial::~wxDial()
{}

void wxDial::enterEvent(wxMouseEvent& event)
{
   std::cout << __func__ << " :: m_MousePos(" << m_MousePosY << "," << m_MousePosY << ")" << std::endl;
   startTimer( 35 );
}
void wxDial::leaveEvent(wxMouseEvent& event)
{
   std::cout << __func__ << " :: m_MousePos(" << m_MousePosY << "," << m_MousePosY << ")" << std::endl;
   stopTimer();
}
void wxDial::startTimer(int milliseconds)
{
   std::cout << "startTimer(" << milliseconds << ")" << std::endl;
   m_Timer.Start( milliseconds );
}
void wxDial::stopTimer()
{
   std::cout << "stopTimer()" << std::endl;
   m_Timer.Stop();
}
void wxDial::timerEvent(wxTimerEvent &event)
{
   //DE_DEBUG("")
   if ( &event.GetTimer() == &m_Timer )
   {
      //DE_DEBUG("")
      Refresh( false );
      event.Skip();
   }
}
void wxDial::backgroundEvent( wxEraseEvent& event )
{
   //DE_DEBUG("")
//   if ( event.GetDC() )
//   {
//      render( *event.GetDC() );
//   }
   event.Skip();
}
void wxDial::paintEvent( wxPaintEvent & event )
{
   //DE_DEBUG("")
   wxAutoBufferedPaintDC dc( this );
   //PrepareDC( dc );
   //updateDstImage();

   int32_t w = int32_t( GetClientSize().x );
   int32_t h = int32_t( GetClientSize().y );
   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }
   wxColour bg(50,50,50);
   wxColour white(255,255,255);
   wxColour black(0,0,0);
   wxColour red(255,0,0);
   wxColour green(0,255,0);
   wxColour blue(0,0,255);
   wxColour orange(255,155,55);

   int x = 0;
   int y = 0;
   dc.SetPen( wxNullPen );
   dc.SetBrush( wxBrush( randomWxColor(), wxBRUSHSTYLE_SOLID ) );
   dc.DrawRectangle( 0, 0, w, h );

//   int x = w/2;
//   int y = h/2 - 8;
//   int r = std::min( x, y ) - 6;
//   dc.SetPen( wxPen( black ) );
//   dc.SetBrush( wxBrush( white, wxBRUSHSTYLE_SOLID ) );
//   dc.DrawCircle( x, y, r);

//   dc.DrawLine( x,0,x,h-1 ); // h-center
//   dc.DrawLine( 0,y,w-1,y ); // v-center

   int k = ((m_Value-m_Min) / (m_Max - m_Min) * h);
   y = h - 1 - k;
   dc.SetPen( wxNullPen );
   dc.SetBrush( wxBrush( green, wxBRUSHSTYLE_SOLID ) );
   dc.DrawRectangle( 0, y, w, k );


   dc.DrawLine( 0,y,w-1,y ); // h-center


   // Draw Title:
   wxFont small8( 8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false );
   dc.SetFont( small8 );
   dc.SetTextForeground( white );

   wxSize ts = dc.GetTextExtent( m_Title );
   x = (w - ts.GetWidth())/2;
   y = h - ts.GetHeight() - 2; // (h - ts.GetHeight())/2;
   dc.DrawText( m_Title, wxPoint( x,y ) );

   //wxString s = wxString::Format("%d x %d",w,h);
   //dc.SetPen( wxPen( wxColour( 20, 100, 20 ), 1, wxPENSTYLE_SOLID ) );
   //dc.SetBrush( wxBrush( wxColour( 55, 200, 55 ), wxBRUSHSTYLE_SOLID ) );
   //dc.DrawText( s, w/2, h/2 );

   // Find Out where the window is scrolled to
//    int vbX,vbY;                     // Top left corner of client
//    GetViewStart(&vbX,&vbY);
//    int vX,vY,vW,vH;                 // Dimensions of client area in pixels
//    wxRegionIterator upd(GetUpdateRegion()); // get the update rect list
//    while (upd)
//    {
//        vX = upd.GetX(); vY = upd.GetY(); vW = upd.GetW(); vH = upd.GetH();
//        // Alternatively we can do this:
//        // wxRect rect(upd.GetRect());
//        // Repaint this rectangle
//        ...some code...
//        upd ++ ;
//    }
   event.Skip();
}

void wxDial::resizeEvent( wxSizeEvent & event )
{
   //int32_t w = int32_t( event.GetSize().x );
   //int32_t h = int32_t( event.GetSize().y );
   //DE_DEBUG( "w = ", w, ", h = ", h )

   Refresh( false );
   event.Skip();
}

void wxDial::mouseEvent(wxMouseEvent& event)
{
   m_MousePosX = event.GetX();
   m_MousePosY = event.GetY();
   //std::cout << __func__ << " :: mousePos(" << mx << ","<< my << ")\n";

   int mmx = m_MousePosX - m_LastMousePosX;
   int mmy = m_MousePosY - m_LastMousePosY;

   m_LastMousePosX = m_MousePosX;
   m_LastMousePosY = m_MousePosY;
   //std::cout << __func__ << " :: mousePos(" << mx << ","<< my << ")\n";


   if (event.IsButton())
   {
      if (event.LeftDown())
      {
         m_IsDragging = true;
         m_MouseDragBegPosX = m_MousePosX;
         m_MouseDragBegPosY = m_MousePosY;
         //startTimer( 16 );
      }
      else if (event.LeftUp())
      {
         m_IsDragging = false;
         m_MouseDragEndPosX = m_MousePosX;
         m_MouseDragEndPosY = m_MousePosY;
         //stopTimer();
      }
   }

//   if ( m_IsDragging )
//   {
//      m_Value =
//   }
//   if (event.IsButton())
//   {
//      if (event.LeftDown()) sevt.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
//      else if (event.LeftUp()) sevt.MouseInput.Event = irr::EMIE_LMOUSE_LEFT_UP;
//      else if (event.MiddleDown()) sevt.MouseInput.Event = irr::EMIE_MMOUSE_PRESSED_DOWN;
//      else if (event.MiddleUp()) sevt.MouseInput.Event = irr::EMIE_MMOUSE_LEFT_UP;
//      else if (event.RightDown()) sevt.MouseInput.Event = irr::EMIE_RMOUSE_PRESSED_DOWN;
//      else if (event.RightUp()) sevt.MouseInput.Event = irr::EMIE_RMOUSE_LEFT_UP;
//   }
//   else if (event.GetWheelRotation() != 0)
//   {
//      sevt.MouseInput.Wheel = (float)event.GetWheelRotation();
//      sevt.MouseInput.Event = irr::EMIE_MOUSE_WHEEL;
//   }
//   else if (event.Moving() || event.Dragging())
//   {
//		sevt.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
//	}
   event.Skip();
}

void wxDial::keyPressEvent(wxKeyEvent& event)
{
   // std::cout<<"wxIrrlichtControl::OnKey()"<<std::endl;
//   irr::SEvent sevt;
//   sevt.EventType = irr::EET_KEY_INPUT_EVENT;
//   sevt.KeyInput.Key = (irr::EKEY_CODE)event.GetRawKeyCode();
//   sevt.KeyInput.PressedDown = event.GetEventType() == wxEVT_KEY_DOWN;
//   sevt.KeyInput.Shift = event.ShiftDown();
//   sevt.KeyInput.Control = event.CmdDown();
//   #if wxUSE_UNICODE
//   sevt.KeyInput.Char = event.GetUnicodeKey();
//   #else
//   sevt.KeyInput.Char = event.GetKeyCode();
//   #endif

//   if ( m_Device )
//   {
//      m_Device->postEventFromUser(sevt);
//   }

//   if ( !m_Timer.IsRunning() )
//   {
//      render();
//   }

   event.Skip();
}

void wxDial::keyReleaseEvent(wxKeyEvent& event)
{
   // std::cout<<"wxIrrlichtControl::OnKey()"<<std::endl;
//   irr::SEvent sevt;
//   sevt.EventType = irr::EET_KEY_INPUT_EVENT;
//   sevt.KeyInput.Key = (irr::EKEY_CODE)event.GetRawKeyCode();
//   sevt.KeyInput.PressedDown = !(event.GetEventType() == wxEVT_KEY_UP);
//   sevt.KeyInput.Shift = event.ShiftDown();
//   sevt.KeyInput.Control = event.CmdDown();
//   #if wxUSE_UNICODE
//   sevt.KeyInput.Char = event.GetUnicodeKey();
//   #else
//   sevt.KeyInput.Char = event.GetKeyCode();
//   #endif

//   if ( m_Device )
//   {
//      m_Device->postEventFromUser(sevt);
//   }

//   if ( !m_Timer.IsRunning() )
//   {
//      render();
//   }

   event.Skip();
}



//void wxDial::focusEnterEvent(wxFocusEvent& event)
//{
//   std::cout << __func__ << " :: m_MousePos(" << m_MousePosY << "," << m_MousePosY << ")" << std::endl;
//   //startTimer( 16 );
//}
//void wxDial::focusLeaveEvent(wxFocusEvent& event)
//{
//   std::cout << __func__ << " :: m_MousePos(" << m_MousePosY << "," << m_MousePosY << ")" << std::endl;
//   stopTimer();
//}
