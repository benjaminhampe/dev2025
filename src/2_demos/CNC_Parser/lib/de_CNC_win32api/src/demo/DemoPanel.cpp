#include <demo/DemoPanel.hpp>

namespace {

COLORREF gDemoPanelColor = RGB(200,200,200);

void setDemoPanelColor( int r, int g, int b )
{
   gDemoPanelColor = RGB(r,g,b);
}

LRESULT CALLBACK
DemoPanelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
      case WM_PAINT:
      {
         RECT rect;
         GetClientRect(hwnd, &rect);

         PAINTSTRUCT ps;
         HDC hdc = BeginPaint(hwnd, &ps);
         SetBkColor(hdc, gDemoPanelColor);
         ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rect, L"Hello World!", 0, NULL);
         EndPaint(hwnd, &ps);
      } break;
   }
   return DefWindowProc(hwnd, msg, wParam, lParam);
}

void
registerDemoPanel()
{
   WNDCLASSW wc = {0};
   wc.lpszClassName = L"DemoPanel";
   wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
   wc.lpfnWndProc = DemoPanelProc;
   RegisterClassW(&wc);
}

} // end namespace <empty>

HWND
createDemoPanel( int x, int y, int w, int h, HWND parent, int id )
{
   registerDemoPanel();
   HWND panel = CreateWindowW(L"DemoPanel", nullptr, WS_CHILD | WS_VISIBLE,
               x,y,w,h, parent, reinterpret_cast<HMENU>(u64(id)), nullptr, nullptr);
   return panel;
}
