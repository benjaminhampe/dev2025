#include <demo/DemoSpinBox.hpp>
#include <commctrl.h>          // Common controls (includes UPDOWN_CLASSW)

namespace {

/*
void UpdateLabel()
{
   LRESULT pos = SendMessageW(g_app.m_trackBar, TBM_GETPOS, 0, 0);
   wchar_t buf[4];
   //wsprintfW(buf, L"%ld", pos);
   StringCbPrintfW(buf, 4, L"%ld", pos);
   SetWindowTextW( g_app.m_edtFeedSpeed, buf);
}

LRESULT CALLBACK
handleDemoSpinBoxProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
   case WM_NOTIFY:
   {
      int code = ((LPNMHDR) lParam)->code;
      if (code == UDN_DELTAPOS)
      {
         NMUPDOWN* spinData = reinterpret_cast<NMUPDOWN*>(lParam);
         int value = spinData->iPos + spinData->iDelta;
//         if (value < ID_SPINBOX_FEEDSPEED_MIN)
//         {
//            value = ID_SPINBOX_FEEDSPEED_MIN;
//         }
//         if (value > ID_SPINBOX_FEEDSPEED_MAX)
//         {
//            value = ID_SPINBOX_FEEDSPEED_MAX;
//         }
//         const int asize = 4;
//         wchar_t buf[asize];
//         size_t cbDest = asize * sizeof(wchar_t);
//         StringCbPrintfW(buf, cbDest, L"%d", value);

         std::wostringstream o;
         o << value;

         SetWindowTextW(g_app.m_edtFeedSpeed, o.str().c_str());
      }
   }
      break;
   case WM_HSCROLL:
      UpdateLabel();
      break;
       break;
   }
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
*/

} // end namespace <empty>

HWND
createDemoSpinBox( int x, int y, int w, int h, HWND parent, int id )
{
   HWND spinBox = CreateWindowW(UPDOWN_CLASSW,
                                nullptr,
                                WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT | UDS_ALIGNRIGHT,
                                x, y, w, h, parent,
                                reinterpret_cast<HMENU>(id), nullptr, nullptr );

//   HWND hEdit = CreateWindowExW(WS_EX_CLIENTEDGE,
//                                WC_EDITW,
//                                nullptr,
//                                WS_CHILD | WS_VISIBLE | ES_RIGHT,
//                                20, 430, 70, 25, parent,
//                                 reinterpret_cast<HMENU>(ID_SPINBOX_BUDDY1), nullptr, nullptr );

//   HWND hStatic = CreateWindowW(WC_STATICW,
//                                L"0",
//                                WS_CHILD | WS_VISIBLE | SS_LEFT,
//                                20, 500, 300, 230, parent,
//                                reinterpret_cast<HMENU>(ID_SPINBOX_BUDDY2), nullptr, nullptr );

//   SendMessageW(spinBox, UDM_SETBUDDY, reinterpret_cast<WPARAM>(hEdit), 0);
//   SendMessageW(spinBox, UDM_SETRANGE, 0,
//                MAKELPARAM(ID_SPINBOX_FEEDSPEED_MAX, ID_SPINBOX_FEEDSPEED_MIN));
//   SendMessageW(spinBox, UDM_SETPOS32, 0, 0);

   return spinBox;
}
