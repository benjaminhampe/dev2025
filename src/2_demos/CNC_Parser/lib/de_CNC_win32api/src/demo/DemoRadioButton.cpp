#include <demo/DemoRadioButton.hpp>

namespace {



} // end namespace <empty>

/*
Button*
createDemoRadioButton( std::wstring caption, int x, int y, int w, int h, HWND parent, int id,
                  std::function< void() > const & callback )
{
   Button* button = new Button();
   button->m_id = id;
   button->m_callback = callback;
   button->m_hwnd = CreateWindowW( L"Button",
                  caption.c_str(),
                  WS_VISIBLE | WS_CHILD, //  | BS_CHECKBOX
                  x,y,w,h, parent,
                  reinterpret_cast<HMENU>(id), nullptr, nullptr );

   SetWindowLongPtr( button->m_hwnd, GWL_USERDATA,
                     reinterpret_cast< LONG_PTR >( button ) );
   return button;
}
*/
