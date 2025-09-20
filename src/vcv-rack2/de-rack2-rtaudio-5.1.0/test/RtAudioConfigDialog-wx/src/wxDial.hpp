#ifndef DE_WX_LEVELMETER_CTRL_HPP
#define DE_WX_LEVELMETER_CTRL_HPP

#include <wxImageCtrl.hpp>

struct wxDial : public wxControl
{
   wxDial(
      wxWindow* parent,
      wxWindowID id,
      wxString const & title,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL );

   ~wxDial();

   void timerEvent( wxTimerEvent& event );
   void backgroundEvent( wxEraseEvent& event );
   void paintEvent( wxPaintEvent & event );
   void resizeEvent( wxSizeEvent & event );
   void mouseEvent( wxMouseEvent& event );
   void keyPressEvent( wxKeyEvent& event );
   void keyReleaseEvent( wxKeyEvent& event );
//   void focusEnterEvent(wxFocusEvent& event );
//   void focusLeaveEvent(wxFocusEvent& event );
   void enterEvent( wxMouseEvent& event );
   void leaveEvent( wxMouseEvent& event );
   void startTimer( int milliseconds );
   void stopTimer();

   wxString m_Title;
   float m_Value;
   float m_Min;
   float m_Max;
   uint32_t m_Steps;

   int m_MousePosX;
   int m_MousePosY;
   int m_LastMousePosX;
   int m_LastMousePosY;

   int m_MouseDragBegPosX;
   int m_MouseDragBegPosY;
   int m_MouseDragEndPosX;
   int m_MouseDragEndPosY;
   bool m_IsDragging;

   wxTimer m_Timer;
private:
   DECLARE_EVENT_TABLE()
};

#endif // Frame_H

