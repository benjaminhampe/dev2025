#include <demo/DemoTextArea.hpp>

namespace {

} // end namespace <empty>

HWND
createDemoTextArea( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND textArea = CreateWindowExW( WS_EX_RIGHTSCROLLBAR,
                     L"edit",
                     caption.c_str(),
                     WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE,
                     x, y, w, h, parent, reinterpret_cast<HMENU>(id), nullptr, nullptr );

//   SetWindowTextW( textArea, L"Hello World\r\n"
//      L"Hello World\r\n"

   return textArea;
}
