#include "wxTouchComboBox.hpp"

//BEGIN_EVENT_TABLE( wxTouchComboBox, wxControl )
//   EVT_TIMER(ID_UPDATE_TIMER, wxTouchComboBox::timerEvent )
//   //   EVT_TIMER(BENNI_ID_FPS_TIMER, wxTouchComboBox::timerEvent)
//   EVT_SIZE( wxTouchComboBox::resizeEvent )
//   EVT_PAINT( wxTouchComboBox::paintEvent )
//   //EVT_ERASE_BACKGROUND( wxTouchComboBox::backgroundEvent )
//   //   EVT_MOUSE_EVENTS(wxTouchComboBox::mouseEvent)
//   //   EVT_KEY_DOWN(wxTouchComboBox::keyPressEvent)
//   //   EVT_KEY_UP(wxTouchComboBox::keyReleaseEvent)
//END_EVENT_TABLE()

wxTouchComboBox::wxTouchComboBox(
      wxWindow* parent,
      wxWindowID id,
      wxPoint const & pos,
      wxSize const & size,
      long style )
   : wxControl( parent, id, pos, size, style, wxDefaultValidator )
   , m_selected( -1 )
{
   m_img = new wxImageCtrl( this, wxID_ANY );
   m_img->SetMinSize( wxSize( 42,42) );
   m_edt = new wxTextCtrl( this, wxID_ANY, _("Wasapi") );
   m_edt->SetMinSize( wxSize( 250,42) );
   m_btn = new wxButton( this, wxID_ANY, _("v") );
   m_btn->SetMinSize( wxSize( 42,42) );
   //wxButton* m_btnDefault;
   //wxButton* m_btnUpdate;

   wxSizerFlags f = wxSizerFlags().Border(wxALL,0).CenterVertical(); //.FixedMinSize();
   wxBoxSizer* h = new wxBoxSizer( wxHORIZONTAL );
   h->Add( m_img, f );
   h->Add( m_edt, f );
   h->Add( m_btn, f );

   SetSizerAndFit( h );
}

wxTouchComboBox::~wxTouchComboBox()
{}
