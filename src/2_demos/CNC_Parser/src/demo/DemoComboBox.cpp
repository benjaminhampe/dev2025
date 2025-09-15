#include <demo/DemoComboBox.hpp>

namespace {

/*
LRESULT CALLBACK
handleComboBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   wchar_t const * items[] = { L"FreeBSD", L"OpenBSD",
   L"NetBSD", L"Solaris", L"Arch" };

   switch(msg)
   {
   case WM_COMMAND:
   {
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
         LRESULT sel = SendMessage(m_comboBox, CB_GETCURSEL, 0, 0);
         //SetWindowTextW(hwndStatic, items[sel]);
         dbMessageW(items[sel],L"CBN_SELCHANGE");
      }
      break;
   }
   case WM_HSCROLL:
      { UpdateLabel(); } break;
   default:
      break;
   }
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
*/

} // end namespace <empty>

HWND
createComboBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND comboBox = CreateWindowW( L"Combobox",
                                  caption.c_str(),
                                  WS_CHILD | WS_VISIBLE | CBS_DROPDOWN,
                                  x,y,w,h,parent,
                                  reinterpret_cast<HMENU>(id), nullptr, nullptr );
//   for (int i = 0; i < 4; i++ )
//   {
//      SendMessageW(comboBox, CB_ADDSTRING, 0, (LPARAM) items[i]);
//   }

   return comboBox;
}
