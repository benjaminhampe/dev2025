#ifndef DE_WX_LEVELMETER_CTRL_HPP
#define DE_WX_LEVELMETER_CTRL_HPP

#include <wxImageCtrl.hpp>

class wxLevelMeter : public wxControl
{
public:
   //DE_CREATE_LOGGER("wxLevelMeter")

   wxLevelMeter(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL );

   ~wxLevelMeter();

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
   //void setImage( de::Image const & img );
   void setImage( wxImage const & img );
   void setFillColor( wxColour const & color );
   void setPreserveAspectRatio( bool preserve );
   void setScaleEnabled( bool enabled );
   void setScaleQuality( wxImageResizeQuality quality );
   void updateDstImage();
   void render( wxPaintDC & dc );

   wxTimer m_Timer;
   bool m_IsDirty;
   bool m_IsScalingEnabled;
   bool m_IsPreservingAspect;
   wxColour m_FillColor;
   wxImageResizeQuality m_ScaleQuality;
   wxImage m_SrcImg;
   wxImage m_DstImg;
   wxBitmap m_DstBmp;


private:
   DECLARE_EVENT_TABLE()
};

#endif // Frame_H

