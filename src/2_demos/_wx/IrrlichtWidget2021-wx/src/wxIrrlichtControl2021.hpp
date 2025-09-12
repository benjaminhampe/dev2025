/// @brief wxIrrlichtControl.hpp

/// @author Benjamin Hampe <benjaminhampe@gmx.de>

/// (c) 2011 - 2021, free software. A wxWidgets control that offers the 'Irrlicht' 3D Engine as hardware accelerated canvas.

#ifndef DE_WX_IRRLICHT_CONTROL_2021_HPP
#define DE_WX_IRRLICHT_CONTROL_2021_HPP

#include <irrlicht.h>
#include <wx/wx.h>
#include <cstdint>
#include <sstream>
#include <iostream>

class wxIrrlichtControl : public wxControl
{
   enum { ID_IRR_TIMER=1000 };

   irr::SIrrlichtCreationParameters m_CreateParams;
   irr::IrrlichtDevice* m_Device;
   irr::scene::ICameraSceneNode* m_Camera;
   wxTimer m_Timer;
   irr::gui::IGUIStaticText* m_FPSLabel;
   bool m_ShowFPSLabel;
public:
   wxIrrlichtControl(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL,
      wxString const & name = wxPanelNameStr );

   ~wxIrrlichtControl();

   void init();
   void render();
   void startTimer(int milliseconds = 10);
   void stopTimer();

   irr::IrrlichtDevice* getIrrlichtDevice() const { return m_Device; }

protected:
   void eraseBackgroundEvent(wxEraseEvent& event);
   void paintEvent(wxPaintEvent& event);
   void resizeEvent(wxSizeEvent& event);
   void timerEvent(wxTimerEvent& event);
   void mouseEvent(wxMouseEvent& event);
   void keyPressEvent(wxKeyEvent& event);
   void keyReleaseEvent(wxKeyEvent& event);
   void enterEvent( wxMouseEvent & event );
   void leaveEvent( wxMouseEvent & event );
private:

   DECLARE_EVENT_TABLE()

};

#endif
