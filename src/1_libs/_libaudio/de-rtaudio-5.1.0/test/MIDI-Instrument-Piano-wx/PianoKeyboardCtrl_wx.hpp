#ifndef DE_RTMIDI_PIANO_KEYBOARD_WIDGET_WX_HPP
#define DE_RTMIDI_PIANO_KEYBOARD_WIDGET_WX_HPP

#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/artprov.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/bmpcbox.h>  // wxComboBox
#include <wx/bitmap.h>
#include <wx/dialog.h>
// #include <wx/filectrl.h>
// #include <wx/filepicker.h>
// #include <wx/filedlg.h>
// #include <wx/image.h>
// #include <wx/stattext.h>
// #include <wx/textctrl.h>
// #include <wx/menu.h>
// #include <wx/menuitem.h>
// #include <wx/dirctrl.h>
// #include <wx/font.h>
// #include <wx/buffer.h> // wxWCharBuffer
#include <wx/tglbtn.h>
#include <wx/log.h>
#include <wx/sizer.h>

#include <DarkMidi.hpp>

enum {
   TimerID_DrawPiano = wxID_HIGHEST+1,
   MenuID_FileExit,
   MenuID_FileLoad,
   // MenuID_FileSAVE,
   // MenuID_FileSAVE_AS,
   MenuID_HelpAbout,
};

// ===================================================================
struct PianoKeyboardCtrl : public wxControl
{
   DE_CREATE_LOGGER("PianoKeyboardCtrl")

   PianoKeyboardCtrl( wxWindow* parent, wxWindowID id, wxPoint const & pos = wxDefaultPosition,
             wxSize const & size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
   ~PianoKeyboardCtrl() override;

// protected:
   void timerEvent( wxTimerEvent& event);
   void backgroundEvent( wxEraseEvent& event );
   void paintEvent( wxPaintEvent& event);
   void resizeEvent( wxSizeEvent& event);
   void mouseEvent( wxMouseEvent& event );
   void keyPressEvent( wxKeyEvent& event );
   void keyReleaseEvent( wxKeyEvent& event );
   void enterEvent( wxMouseEvent& event );
   void leaveEvent( wxMouseEvent& event );



private:
   wxTimer m_Timer;
   bool m_IsEntered;

   std::array< bool, 256 > m_KeyStates;
   glm::ivec2 m_MousePos;
   glm::ivec2 m_MousePosLast;
   glm::ivec2 m_MouseMove;

   RtMidiEngine* m_Engine;

   DECLARE_EVENT_TABLE()
};


#endif

