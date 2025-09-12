#include "AboutDialog.hpp"
#include <../res/resource.h>

namespace {

INT_PTR CALLBACK
AboutDialogProc( HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
   switch(Message)
   {
   case WM_INITDIALOG:
      return TRUE;
   case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
         EndDialog(hwnd, IDOK);
      break;
      case IDCANCEL:
         EndDialog(hwnd, IDCANCEL);
      break;
      }
      break;
   default:
      return FALSE;
   }
   return TRUE;
}

} // end namespace <empty>

void doModalAboutDialog( HWND parent )
{
   auto ret = DialogBoxParam( GetModuleHandle(nullptr),
                     MAKEINTRESOURCE(IDD_ABOUTDIALOG),
                     parent, AboutDialogProc, LPARAM(0) );
   if ( ret == IDOK )
   {
//      MessageBox( hwnd,
//                  TEXT("Dialog exited with IDOK."),
//                  TEXT("Notice"),
//                  MB_OK | MB_ICONINFORMATION);
   }
   else if( ret == IDCANCEL )
   {
//      MessageBox( hwnd,
//                  TEXT("Dialog exited with IDCANCEL."),
//                  TEXT("Notice"), MB_OK | MB_ICONINFORMATION);
   }
   else if( ret == -1 )
   {
//      MessageBox( hwnd,
//                  TEXT("Dialog failed!"),
//                  TEXT("Error"), MB_OK | MB_ICONINFORMATION);
   }
}
