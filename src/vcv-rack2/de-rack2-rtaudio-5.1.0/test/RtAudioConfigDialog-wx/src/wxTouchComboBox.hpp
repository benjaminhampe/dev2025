#ifndef DE_WX_TOUCH_COMBOBOX_HPP
#define DE_WX_TOUCH_COMBOBOX_HPP

#include <wx/wx.h>
#include <wxImageCtrl.hpp>

class wxTouchComboBox : public wxControl
{
public:
   //DE_CREATE_LOGGER("wxTouchComboBox")

   wxTouchComboBox(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos = wxDefaultPosition,
      wxSize const & size = wxDefaultSize,
      long style = wxTAB_TRAVERSAL );

   ~wxTouchComboBox();

      wxImageCtrl* m_img;       // Hardware icon
      wxTextCtrl* m_edt;
      wxButton* m_btn;
      //wxButton* m_btnDefault;
      //wxButton* m_btnUpdate;
      struct Item {
         wxImage img;
         wxString msg;
      };
      std::vector< Item > m_items;
      int m_selected;

   void Append( wxString const & msg, wxImage const & img = wxNullImage )
   {
      Item item;
      item.img = img;
      item.msg = msg;
      m_items.emplace_back( std::move( item ) );
      if ( m_selected < 0 ) m_selected = int( m_items.size() ) - 1;
   }

   void Select( int index )
   {
      if ( index < 0 || index >= m_items.size() )
      {
         //DE_ERROR("Invalid index ", index)
         return;
      }
      if ( m_selected == index )
      {
         //DE_DEBUG("Nothing todo, selected index ", index)
         return;
      }

      m_selected = index;
      m_img->setImage( m_items[ index ].img );
      m_edt->SetValue( m_items[ index ].msg );
   }

   int GetCurrentSelection() const
   {
      return m_selected;
   }

   //void timerEvent( wxTimerEvent& event );
   //void backgroundEvent( wxEraseEvent& event );
   //void paintEvent( wxPaintEvent & event );
   //void resizeEvent( wxSizeEvent & event );
   //void OnButtonClicked( wxCommandEvent & event )
   //   wxTimer m_Timer;

private:
   // DECLARE_EVENT_TABLE()
};

#endif // Frame_H

