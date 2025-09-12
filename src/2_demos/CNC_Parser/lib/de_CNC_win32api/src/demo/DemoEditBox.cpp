#include <demo/DemoEditBox.hpp>

namespace {

} // end namespace <empty>

HWND
createDemoEditBox( std::wstring caption, int x, int y, int w, int h, HWND parent, int id )
{
   HWND editBox = CreateWindowW( L"Edit",
                                 caption.c_str(),
                                 WS_CHILD | WS_VISIBLE | WS_BORDER,
                                 x, y, w, h, parent,
                                 reinterpret_cast<HMENU>(id), nullptr, nullptr );
   return editBox;
}

std::wstring
getEditBoxTextW( HWND editBox )
{
   //PostQuitMessage(0);
   int len = GetWindowTextLengthW(editBox) + 1;
   std::vector< wchar_t > tmp( size_t(len) + 1, L'\0' );
   //wchar_t wtext[len];
   GetWindowTextW(editBox, tmp.data(), len);
   tmp[ size_t(len) ] = L'\0';
   std::wstring ws = tmp.data();

   //dbMessageW( ws, L"EditBox FeedSpeed" );

   return ws;

   // SetWindowTextW(hwnd, wtext);
}
