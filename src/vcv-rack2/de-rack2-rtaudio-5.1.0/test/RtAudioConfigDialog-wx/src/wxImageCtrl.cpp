#include "wxImageCtrl.hpp"

BEGIN_EVENT_TABLE( wxImageCtrl, wxControl )
   EVT_TIMER(ID_UPDATE_TIMER, wxImageCtrl::timerEvent )
   //   EVT_TIMER(BENNI_ID_FPS_TIMER, wxImageCtrl::timerEvent)
   EVT_SIZE( wxImageCtrl::resizeEvent )
   EVT_PAINT( wxImageCtrl::paintEvent )
   //EVT_ERASE_BACKGROUND( wxImageCtrl::backgroundEvent )
   //   EVT_MOUSE_EVENTS(wxImageCtrl::mouseEvent)
   //   EVT_KEY_DOWN(wxImageCtrl::keyPressEvent)
   //   EVT_KEY_UP(wxImageCtrl::keyReleaseEvent)
END_EVENT_TABLE()

wxImageCtrl::wxImageCtrl(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxControl( parent, id, pos, size, style, wxDefaultValidator )
   , m_IsDirty( true )
   , m_IsScalingEnabled( false )
   , m_IsPreservingAspect( false )
   , m_Timer( this, ID_UPDATE_TIMER )
   , m_FillColor( 255,255,255 )
   , m_ScaleQuality( wxIMAGE_QUALITY_HIGH )

{
   SetMinSize( wxSize(24,24) );
   Fit();
   //SetDoubleBuffered( true );
   SetBackgroundStyle( wxBG_STYLE_PAINT );
   // m_X->Bind( wxEVT_COMMAND_SPINCTRL_UPDATED, &wxImageCtrl::OnXChanged, this );
   //Connect(wxEVT_SIZE, wxSizeEventHandler(MySubScrolledWindow::OnSize));


   //setImage( createWxImageFromColor( 16,16, 0xFF1080F0 ) );
   // m_Timer.Start( 50 );
}

wxImageCtrl::~wxImageCtrl()
{}

void
wxImageCtrl::setFillColor( wxColour const & color )
{
   if ( m_FillColor == color ) return;
   m_FillColor = color;
   Refresh( false );
}

void
wxImageCtrl::setScaleEnabled( bool enabled )
{
   if ( m_IsScalingEnabled == enabled ) return;
   m_IsScalingEnabled = enabled;
   m_IsDirty = true;
   Refresh( false );
}

void
wxImageCtrl::setPreserveAspectRatio( bool preserve )
{
   if ( m_IsPreservingAspect == preserve ) return;
   m_IsPreservingAspect = preserve;
   m_IsDirty = true;
   Refresh( false );
}

void
wxImageCtrl::setScaleQuality( wxImageResizeQuality quality )
{
   if ( m_ScaleQuality == quality ) return;
   m_ScaleQuality = quality;
   m_IsDirty = true;
   Refresh( false );
}

#ifdef USE_DARKIMAGE
void
wxImageCtrl::setImage( de::Image const & img )
{
   // DE_DEBUG("img ", img.toString())
   m_SrcImg = toWxImage( img );
//   if ( m_SrcImg.IsOk() )
//   {
//      SetMinSize( wxSize( m_SrcImg.GetWidth()+8, m_SrcImg.GetHeight()+8 ) );
//   }
//   else
//   {
//      SetMinSize( wxSize( 16,16 ) );
//   }
   m_IsDirty = true;
   Refresh( false );
}
#endif
void
wxImageCtrl::setImage( wxImage const & img )
{
   //DE_DEBUG("")
   m_SrcImg = img;
//   if ( m_SrcImg.IsOk() )
//   {
//      SetMinSize( wxSize( m_SrcImg.GetWidth()+8, m_SrcImg.GetHeight()+8 ) );
//   }
//   else
//   {
//      SetMinSize( wxSize( 16,16 ) );
//   }
   m_IsDirty = true;
   Refresh( false );
}

void wxImageCtrl::timerEvent(wxTimerEvent &event)
{
   //DE_DEBUG("")
   if ( event.GetTimer().GetId() == ID_UPDATE_TIMER )
   {
      // render( *event.GetDC() );
      //Paint
      Refresh( false );
      event.Skip();
   }
}

void wxImageCtrl::backgroundEvent( wxEraseEvent& event )
{
   //DE_DEBUG("")
//   if ( event.GetDC() )
//   {
//      render( *event.GetDC() );
//   }
   event.Skip();
}

void
wxImageCtrl::updateDstImage()
{
   if ( !m_IsDirty ) return;
   int w = GetClientSize().GetWidth();
   int h = GetClientSize().GetHeight();
   if ( w < 1 ) return;
   if ( h < 1 ) return;
   int src_w = m_SrcImg.GetWidth();
   int src_h = m_SrcImg.GetHeight();

   if ( m_IsScalingEnabled )
   {
      int dst_w = w;
      int dst_h = h;

      if ( m_IsPreservingAspect )
      {
         auto fit = de::AspectRatio::fitAspectPreserving( src_w, src_h, w, h );
         dst_w = fit.x;
         dst_h = fit.y;
      }

      m_DstImg = m_SrcImg.Scale( dst_w, dst_h, m_ScaleQuality );
      m_DstBmp = wxBitmap( m_DstImg );

   }
   else
   {
      m_DstBmp = wxBitmap( m_SrcImg );
   }

//   int bmp_w = m_DstBmp.GetWidth();
//   int bmp_h = m_DstBmp.GetHeight();
//   DE_DEBUG( "Src image w = ", src_w, ", h = ", src_h )
//   DE_DEBUG( "Dst image w = ", w, ", h = ", h )
//   DE_DEBUG( "Dst bitmap w = ", bmp_w, ", h = ", bmp_h )
   m_IsDirty = false;
}


void wxImageCtrl::paintEvent( wxPaintEvent & event )
{
   //DE_DEBUG("")
   wxAutoBufferedPaintDC dc( this );
   //PrepareDC( dc );
   updateDstImage();
   int32_t w = int32_t( GetClientSize().x );
   int32_t h = int32_t( GetClientSize().y );
   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }
//   DE_DEBUG("Client.w = ", GetClientRect().GetWidth() )
//   DE_DEBUG("Client.w = ", GetClientRect().GetHeight() )
//   DE_DEBUG("GetSize.x = ", w )
//   DE_DEBUG("GetSize.y = ", h )
   dc.SetPen( wxNullPen );
   dc.SetBrush( wxBrush( m_FillColor, wxBRUSHSTYLE_SOLID ) );
   dc.DrawRectangle( 0, 0, w, h );

   int dst_w = m_DstBmp.GetWidth();
   int dst_h = m_DstBmp.GetHeight();
   int dst_x = std::max( 0, (w - dst_w)/2 );
   int dst_y = std::max( 0, (h - dst_h)/2 );
   dc.DrawBitmap( m_DstBmp, wxPoint( dst_x, dst_y ) );

// DEBUG
   //dc.DrawLine( 0,0,w-1,h-1 );
   //dc.DrawLine( 0,h-1,w-1,0 );
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


void wxImageCtrl::resizeEvent( wxSizeEvent & event )
{
   int32_t w = int32_t( event.GetSize().x );
   int32_t h = int32_t( event.GetSize().y );

   //DE_DEBUG( "w = ", w, ", h = ", h )

   if ( w < 1 ) { return; }
   if ( h < 1 ) { return; }

   m_IsDirty = true;
   Refresh( false );
   event.Skip();
}

//void wxImageCtrl::OnButtonClicked( wxCommandEvent & event )
//{
//   int id = event.GetId();
//   //m_Buttons[ index ]->SetLabel( std::to_string( mineCount ) );
//   //m_Buttons[ index ] -> Disable();

//   event.Skip();
//}
