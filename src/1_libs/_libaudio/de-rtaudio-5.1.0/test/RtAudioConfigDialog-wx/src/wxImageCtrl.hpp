#pragma once
#include <benni_common.hpp>
#include <wx/wx.h>
#include <wx/dcbuffer.h>

class wxImageCtrl : public wxControl
{
public:
   //DE_CREATE_LOGGER("wxImageCtrl")

   wxImageCtrl(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL );

   ~wxImageCtrl();

   enum {
      ID_UPDATE_TIMER = 10234
   };

   void timerEvent( wxTimerEvent& event );
   void backgroundEvent( wxEraseEvent& event );
   void paintEvent( wxPaintEvent & event );
   void resizeEvent( wxSizeEvent & event );
   //void OnButtonClicked( wxCommandEvent & event )
   //int ClientH(float pct) { return (int) (GetClientSize().y * pct / 100); }
   //int ClientW(float pct) { return (int) (GetClientSize().x * pct / 100); }
#ifdef USE_DARKIMAGE
   void setImage( de::Image const & img );
#endif
   void setImage( wxImage const & img );
   void setFillColor( wxColour const & color );
   void setPreserveAspectRatio( bool preserve );
   void setScaleEnabled( bool enabled );
   void setScaleQuality( wxImageResizeQuality quality );
   void updateDstImage();
   void render( wxPaintDC & dc );


   bool m_IsDirty;
   bool m_IsScalingEnabled;
   bool m_IsPreservingAspect;
   wxTimer m_Timer;
   wxColour m_FillColor;
   wxImageResizeQuality m_ScaleQuality;
   wxBitmap m_DstBmp;
   wxImage m_SrcImg;
   wxImage m_DstImg;

private:
   DECLARE_EVENT_TABLE()
};
