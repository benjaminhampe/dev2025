#include <demo/DemoSlider.hpp>

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
handleDemoSliderProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_PAINT:
      {
         RECT rect;
         GetClientRect(hwnd, &rect);

         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hwnd, &ps);
         SetBkColor(hdc, gDemoSliderColor);
         ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, "Hello World!", 0, NULL);
         EndPaint(hwnd, &ps);
      } break;
   }
   return DefWindowProc(hwnd, msg, wParam, lParam);
}
*/

} // end namespace <empty>

HWND
createDemoSlider( int x, int y, int w, int h, HWND parent, int id )
{
   HWND hLeftLabel = CreateWindowW(L"Static",
                                  L"0",
                                  WS_CHILD | WS_VISIBLE,
                                  0, 0, 10, 30, parent,
                                  reinterpret_cast<HMENU>(id+1), nullptr, nullptr );
   HWND hRightLabel = CreateWindowW(L"Static",
                                   L"100",
                                   WS_CHILD | WS_VISIBLE,
                                   0, 0, 30, 30, parent,
                                   reinterpret_cast<HMENU>(id+2), nullptr, nullptr );

   HWND slider = CreateWindowW(TRACKBAR_CLASSW,
                                   L"Trackbar Control",
                                   WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
                                   x, y, w, h, parent,
                                   reinterpret_cast<HMENU>(id), nullptr, nullptr );
   SendMessageW(slider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
   SendMessageW(slider, TBM_SETPAGESIZE, 0, 10);
   SendMessageW(slider, TBM_SETTICFREQ, 10, 0);
   SendMessageW(slider, TBM_SETPOS, FALSE, 0);
   SendMessageW(slider, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel);
   SendMessageW(slider, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel);
   return slider;
}
