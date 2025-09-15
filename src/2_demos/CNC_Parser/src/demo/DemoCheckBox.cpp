#include <demo/DemoCheckBox.hpp>

namespace {

/*
LRESULT CALLBACK
handleCheckBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_COMMAND:
      {
         if (HIWORD(wParam) == BN_CLICKED)
         {
            bool checked = IsDlgButtonChecked(hwnd, ID_BTN_SHOWLOGGER);
            if (checked)
            {
               CheckDlgButton(hwnd, ID_BTN_SHOWLOGGER, BST_UNCHECKED);
               SetWindowTextW(hwnd, L"");
            }
            else
            {
               CheckDlgButton(hwnd, ID_BTN_SHOWLOGGER, BST_CHECKED);
               SetWindowTextW(hwnd, L"Check Box");
            }
         }
      } break;
      default:
         break;
   }
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
*/

} // end namespace <empty>

HWND
createCheckBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND checkBox = CreateWindowW( L"Button",
                             caption.c_str(),
                             WS_VISIBLE | WS_CHILD | BS_CHECKBOX,
                             x,y,w,h, parent,
                             reinterpret_cast<HMENU>(id), nullptr, nullptr );
   CheckDlgButton(parent, id, BST_CHECKED);

   return checkBox;
}
