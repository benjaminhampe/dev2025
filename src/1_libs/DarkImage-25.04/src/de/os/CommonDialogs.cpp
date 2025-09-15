#include <de/os/CommonDialogs.h>
#include <de/Color.h>
#include <de/os/VideoModes.h>
//#include <de/Timer.hpp>
//#include <de/window/IWindow.hpp>

#if defined(WIN32) || defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <commdlg.h>
#include <windowsx.h>
#include <shellapi.h>
#include <commctrl.h>
#include <shlobj.h>
#include <wchar.h>

#if 0
#include <mmsystem.h> // For JOYCAPS
#include <windowsx.h>
#if !defined(GET_X_LPARAM)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif
#include <winuser.h>
#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif

#include <Uxtheme.h>
#include <vssym32.h>
#include <CommCtrl.h>
#include <VersionHelpers.h>
#endif

#include "../res/de_resource.h"
/*
int main() {
    // Load a custom icon (Replace "icon.ico" with an actual icon file)
    HICON hIcon = LoadImage(NULL, L"icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

    if (hIcon) {
        SendMessage(GetConsoleWindow(), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    }

    MessageBox(NULL, L"This is a message!", L"Custom Icon Test", MB_OK | MB_ICONINFORMATION);

    return 0;
}
*/

void dbMessageBoxA(const std::string& msg, const std::string& title)
{
    MessageBoxA( nullptr, msg.c_str(), title.c_str(), MB_OK | MB_ICONQUESTION );
}

void dbMessageBoxW(const std::wstring& msg, const std::wstring& title)
{
    MessageBoxW( nullptr, msg.c_str(), title.c_str(), MB_OK | MB_ICONQUESTION );
}

bool dbPromtBoolA(const std::string& msg, const std::string& title)
{
    return IDOK == MessageBoxA( nullptr, msg.c_str(), title.c_str(), MB_OKCANCEL | MB_ICONINFORMATION );
}

bool dbPromtBoolW(const std::wstring& msg, const std::wstring& title)
{
    return IDOK == MessageBoxW( nullptr, msg.c_str(), title.c_str(), MB_OKCANCEL | MB_ICONINFORMATION );
}

struct PromtStrW_DialogData {
    std::wstring title;
    std::wstring editText;
};

INT_PTR CALLBACK
PromtStrW_DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static PromtStrW_DialogData* w; // Speichert die übergebenen Daten

    switch (msg)
    {
    case WM_INITDIALOG:
        w = reinterpret_cast<PromtStrW_DialogData*>(lParam); // Daten abrufen

        if (w)
        {
            SetWindowTextW(hDlg, w->title.c_str()); // Titel setzen
            SetDlgItemTextW(hDlg, PROMDLG_ID_EDIT, w->editText.c_str()); // Body-Text setzen
        }
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            wchar_t text[256];
            GetDlgItemTextW(hDlg, PROMDLG_ID_EDIT, text, 256);
            MessageBoxW(hDlg, text, L"dbPromtStrW", MB_OK);
            EndDialog(hDlg, 0);
        }
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }
        return TRUE;
    }
    return FALSE;
}

//std::wstring
//ShowStringPrompt(HINSTANCE hInstance, const wchar_t* title, const wchar_t* bodyText)

std::wstring dbPromtStrW(const std::wstring& msg, const std::wstring& title)
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    PromtStrW_DialogData dlg;
    dlg.title = title;
    dlg.editText = msg;

    auto hr = DialogBoxParamW(hInstance,
                   MAKEINTRESOURCEW(PROMDLG_IDD),
                   nullptr,
                   PromtStrW_DialogProc,
                   reinterpret_cast<LPARAM>(&dlg));

    if (SUCCEEDED(hr))
    {
        DE_OK("")
    }
    else
    {
        DE_ERROR("hr = ",hr)
        DWORD err = GetLastError();
        wchar_t buf[256];
        wsprintfW(buf, L"DialogBoxParam failed. Error code: %lu", err);
        MessageBoxW(nullptr, buf, L"Error", MB_OK | MB_ICONERROR);
    }

    return dlg.editText;
}



struct PromtStrA_DialogData {
    std::string title;
    std::string editText;
};

INT_PTR CALLBACK
PromtStrA_DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static PromtStrA_DialogData* a; // Speichert die übergebenen Daten

    switch (msg)
    {
    case WM_INITDIALOG:
        a = reinterpret_cast<PromtStrA_DialogData*>(lParam); // Daten abrufen

        if (a)
        {
            SetWindowTextA(hDlg, a->title.c_str()); // Titel setzen
            SetDlgItemTextA(hDlg, PROMDLG_ID_EDIT, a->editText.c_str()); // Body-Text setzen
        }
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            char text[256];
            GetDlgItemTextA(hDlg, PROMDLG_ID_EDIT, text, 256);
            //MessageBoxA(hDlg, text, "dbPromtStrA", MB_OK);
            EndDialog(hDlg, 0);
            a->editText = text;
        }
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }
        return TRUE;
    }
    return FALSE;
}

std::string dbPromtStrA(const std::string& msg, const std::string& title)
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    PromtStrA_DialogData a = { title.c_str(), msg.c_str() };

    DialogBoxParamA(hInstance,
                   MAKEINTRESOURCEA(PROMDLG_IDD),
                   nullptr,
                   PromtStrA_DialogProc,
                   reinterpret_cast<LPARAM>(&a));

    return a.editText;
}



namespace {

    //===========================================================
    // Replace '|' pipe character to '\0'
    //===========================================================
    void replace_to_null_char( std::string & out )
    {
       for ( size_t i = 0; i < out.size(); ++i )
       {
          if ( out[i] == (unsigned char)124 )
               out[i] = 0x00;
       }
    }

    // struct RECTi { int x, y, w, h; };
    // uint32_t RGBA(int r, int g, int b, int a);
    // int RGBA_R(uint32_t color);
    // int RGBA_G(uint32_t color);
    // int RGBA_B(uint32_t color);
    // int RGBA_A(uint32_t color);
    //inline void GetCurrentPath(char* buffer)
    //{
    //	getcwd(buffer, _MAX_PATH);
    //}

    std::string g_Caption;
    std::string g_InitDir;
    de::Recti   g_Rect;
    bool        g_bNewUI;
/*
    HWND        g_EditWindow;
    char        g_CharBuf[MAX_PATH];

    //---------------------------------------------------------------------------
    DWORD RGBA(int r, int g, int b, int a)
    {
        DWORD cR=(DWORD)r;   DWORD cG=(DWORD)g;   DWORD cB=(DWORD)b;   DWORD cA=(DWORD)a;
        DWORD color = (cA << 24) + (cR << 16) + (cG << 8) + cB;
        return color;
    }
    //---------------------------------------------------------------------------
    int RGBA_R(DWORD color)
    {
        DWORD cR = (color & 0x00FF0000) >> 16;		return (int)cR;
    }
    //---------------------------------------------------------------------------
    int RGBA_G(DWORD color)
    {
        DWORD cG = (color & 0x0000FF00) >> 8;	   return (int)cG;
    }
    //---------------------------------------------------------------------------
    int RGBA_B(DWORD color)
    {
        DWORD cB = (color & 0x000000FF);			   return (int)cB;
    }
    //---------------------------------------------------------------------------
    int RGBA_A(DWORD color)
    {
        DWORD cA = (color & 0xFF000000) >> 24;		return (int)cA;
    }
*/

} // end namespace anonymous

/*
//===========================================================
int CALLBACK
CB_W32_OpenFolderProcA(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
//===========================================================
{
   switch( uMsg )
   {
   case BFFM_INITIALIZED: // Look for BFFM_INITIALIZED
   {
      HINSTANCE hInstance = g_pGlob->hInstance;
      RECT Dialog;
      GetWindowRect(hwnd,&Dialog);

   // Position und Dimension
      int cx=GetSystemMetrics(SM_CXSCREEN);
      int cy=GetSystemMetrics(SM_CYSCREEN);
      if (g_Rect.dx<100)			  g_Rect.dx=100;
      if (g_Rect.dy<100)			  g_Rect.dy=100;
      if (g_Rect.dx>cx)			  g_Rect.dx=cx;
      if (g_Rect.dy>cy)			  g_Rect.dy=cy;
      if (g_Rect.x<0)			  g_Rect.x=(cx-g_Rect.dx)/2;
      if (g_Rect.y<0)			  g_Rect.y=(cy-g_Rect.dy)/2;
      if (g_Rect.x>cx-g_Rect.dx)  g_Rect.x=cx-g_Rect.dx;
      if (g_Rect.y>cy-g_Rect.dy)  g_Rect.y=cy-g_Rect.dy;

      POINT p;
      //Set Window Caption
      if (g_pCaption.empty()==false)
         SendMessage(hwnd,WM_SETTEXT, 0, (LPARAM)g_pCaption.c_str());
      // Search for obsolete static text
      HWND static1=FindWindowEx(hwnd,NULL,"STATIC",NULL);
      HWND ListView=FindWindowEx(hwnd,NULL,"SysTreeView32",NULL);
      HWND btn_ok=FindWindowEx(hwnd,NULL,"BUTTON","OK");
      HWND btn_cn=FindWindowEx(hwnd,NULL,"BUTTON","Cancel");
      edit=CreateWindowEx(0,"EDIT",g_InitDir.c_str(),WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL,0,100,100,50,hwnd,0,hInstance,NULL);

      //Get
      MoveWindow(hwnd,g_Rect.x,g_Rect.y,g_Rect.dx,g_Rect.dy,true);
      GetWindowRect(hwnd,&Dialog);

      //Set
      SetWindowPos(static1,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);

      p.x=Dialog.right-Dialog.left-15;
      p.y=Dialog.bottom-Dialog.top-60;
      SetWindowPos(ListView,HWND_BOTTOM,5,5,Dialog.right-Dialog.left-15,p.y-10,0);

      SetWindowPos(edit,HWND_BOTTOM,5,p.y,p.x-150,18,SWP_SHOWWINDOW);
      SetWindowPos(btn_ok,HWND_BOTTOM,p.x-140,p.y,60,20,0);
      SetWindowPos(btn_cn,HWND_BOTTOM,p.x-70,p.y,70,20,0);
      SetFont(edit,"MS Sans Serif",12);
      SetWindowText(edit,g_InitDir.c_str());

      //Init Directory
      strcpy(c,g_InitDir.c_str());
      SendMessage(hwnd, BFFM_SETSELECTION,TRUE,(LPARAM)c);
      //SendMessage(hwnd, BFFM_SETSELECTION,TRUE,(LPARAM)_T("C:\\Program Files"));
      break;
   }

   //Selection change message
   case BFFM_SELCHANGED :
   {
      strcpy(c,g_InitDir.c_str());
      BOOL t = SHGetPathFromIDList((ITEMIDLIST*)lParam, c);
      SetWindowText(edit,c);
      break;
    }
   }
    return 0;
}
//===========================================================
int CALLBACK
CB_W32_OpenFolderProcB(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
//===========================================================
{
   if ( uMsg == BFFM_INITIALIZED ) // Look for BFFM_INITIALIZED
   {
      //Set Window Caption
      if (g_pCaption.empty()==false)
      SendMessage(hwnd,WM_SETTEXT, 0, (LPARAM)g_pCaption.c_str());
      RECT Dialog;
      GetWindowRect(hwnd,&Dialog);

// Nur Position
      int cx=GetSystemMetrics(SM_CXSCREEN);
      int cy=GetSystemMetrics(SM_CYSCREEN);
      if (g_Rect.x<0)				  g_Rect.x=(cx-(Dialog.right-Dialog.left))/2;
      if (g_Rect.y<0)				  g_Rect.y=(cy-(Dialog.bottom-Dialog.top))/2;
      if (g_Rect.x>cx-(Dialog.right-Dialog.left)) g_Rect.x=cx-(Dialog.right-Dialog.left);
      if (g_Rect.y>cy-(Dialog.right-Dialog.left)) g_Rect.y=cy-(Dialog.right-Dialog.left);

      //Sets the listview controls dimentions
      SetWindowPos(hwnd,HWND_BOTTOM,g_Rect.x,g_Rect.y,Dialog.right-Dialog.left,Dialog.bottom-Dialog.top,0);

      //SetWindowPos(hwnd,HWND_BOTTOM,g_Rect.x,g_Rect.y,g_Rect.dx,g_Rect.dx,0);
      //GetWindowRect(hwnd,&Dialog);

      //HWND hdlg=GetParent(hwnd);
      //HWND ListView=FindWindowEx(hdlg,NULL,"SysTreeView32",NULL);
      //HWND ListView=FindWindowEx(hwnd,NULL,"SysTreeView32",NULL);
      //HWND Static1=GetDlgItem(hwnd,14146);
      //SetWindowPos(Static1,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);
      //SetWindowPos(ListView,HWND_BOTTOM,10,5,g_Rect.dx-30,g_Rect.dy-200,0);

      //Set InitDirectory
      strcpy(c,g_InitDir.c_str());
      SendMessage(hwnd, BFFM_SETSELECTION,TRUE,(LPARAM)c);
   }
    return 0;
}


//===========================================================
int CALLBACK
CB_W32_OpenDirProcB(HWND hwnd,UINT uMsg,LPARAM lParam,LPARAM lpData)
//===========================================================
{
   if ( uMsg == BFFM_INITIALIZED ) // Look for BFFM_INITIALIZED
   {
      //Set Window Caption
      if ( !g_Caption.empty() )
      {
         SendMessage( hwnd, WM_SETTEXT, 0, (LPARAM)g_Caption.c_str() );
      }

      RECT r_window;
      GetWindowRect( hwnd, &r_window );

// Nur Position
      int scr_w = GetSystemMetrics(SM_CXSCREEN);
      int scr_h = GetSystemMetrics(SM_CYSCREEN);
      int dlg_w = ( r_window.right - r_window.left )/2;
      int dlg_h = ( r_window.bottom - r_window.top )/2;

      if ( g_Rect.x1 < 0 ) g_Rect.x1 = (scr_w-dlg_w)/2;
      if ( g_Rect.y1 < 0 ) g_Rect.y1 = (scr_h-dlg_h)/2;
      if ( g_Rect.x1 > scr_w - dlg_w) g_Rect.x1 = scr_w - dlg_w;
      if ( g_Rect.y1 > scr_h - dlg_h) g_Rect.y1 = scr_h - dlg_h;

      //Sets the listview controls dimentions
      //SetWindowPos( hwnd, HWND_BOTTOM, g_Rect.x1, g_Rect.y1, dlg_w, dlg_h, 0 );

      SetWindowPos(hwnd,HWND_BOTTOM,g_Rect.x1, g_Rect.y1, g_Rect.getWidth(), g_Rect.getHeight(),0);
      //GetWindowRect( hwnd, &Dialog );
      HWND hdlg=GetParent(hwnd);
      HWND ListView=FindWindowEx(hdlg,NULL,"SysTreeView32",NULL);
      //HWND ListView=FindWindowEx(hwnd,NULL,"SysTreeView32",NULL);
      HWND Static1=GetDlgItem(hwnd,14146);
      SetWindowPos(Static1,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);
      SetWindowPos(ListView,HWND_BOTTOM,10,5,g_Rect.getWidth()-30,g_Rect.getHeight()-200,0);

      //Set InitDirectory
      strcpy( g_CharBuf, g_InitDir.c_str() );
      SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)g_CharBuf );
      SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)g_InitDir.c_str() );
   }
   return 0;
}
*/

int CALLBACK BrowseFolderA_Proc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        // lpData contains the default path as a string
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);

        // Move and Resize dialog:
        if (g_Rect.w > 0 || g_Rect.h > 0)
        {
            if (g_Rect.w <= 0)
            {
                g_Rect.w = dbDesktopWidth() / 2;
                g_Rect.x = dbDesktopWidth() / 4;
            }

            if (g_Rect.h <= 0)
            {
                g_Rect.h = dbDesktopHeight() / 2;
                g_Rect.y = dbDesktopHeight() / 4;
            }

            //HWND hWnd = GetParent( hDlg );
            MoveWindow(hwnd, g_Rect.x, g_Rect.y, g_Rect.w, g_Rect.h, TRUE);
        }
    }
    return 0;
}

//===========================================================
std::string dbOpenFolderDlg( de::BrowseFolderParamsA params )
//===========================================================
{
    g_Rect = de::Recti( params.x, params.y, params.w, params.h );

    params.initDir = de::FileSystem::makeWinPath(params.initDir);

    char outputFolderName[MAX_PATH] = { 0 };

    //////////////////////////////////////////////////////////////////
    // STRUCT BEGIN
    //////////////////////////////////////////////////////////////////
    /*
        hwndOwner	Handle to the parent window (owner of the dialog).
        pidlRoot	Root folder to start browsing from (can be NULL for Desktop).
        pszDisplayName	Pointer to a buffer that receives the display name of the selected folder.
        lpszTitle	Text shown above the folder tree (instructions or prompt).
        ulFlags	Flags to customize dialog behavior (e.g. show edit box, restrict to FS).
        lpfn	Callback function for initialization and interaction.
        lParam	Custom data passed to the callback.
        iImage	Index of the image associated with the selected folder (optional).

        TCHAR szDisplayName[MAX_PATH];
        TCHAR szDefaultPath[] = TEXT("C:\\MyDefaultFolder");

        BROWSEINFO bi = { 0 };
        bi.hwndOwner = hWnd;  // your window handle
        bi.pidlRoot = NULL;   // start from Desktop
        bi.pszDisplayName = szDisplayName;
        bi.lpszTitle = TEXT("Select a folder:");
        bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        bi.lpfn = BrowseCallbackProc;
        bi.lParam = (LPARAM)szDefaultPath;
    */

    BROWSEINFO bi = { 0 }; // memset( &bi, 0, sizeof( bi ) );
    bi.hwndOwner = nullptr; //g_pGlob->hWnd;
    bi.pidlRoot = nullptr;
    bi.pszDisplayName = outputFolderName;
    bi.lpszTitle = params.caption.c_str();
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseFolderA_Proc;
    bi.lParam = (LPARAM)params.initDir.c_str();

    std::string retval;

    LPITEMIDLIST pIDL = SHBrowseForFolder( &bi );
    if ( pIDL )
    {
        char fullPath[_MAX_PATH] = {'\0'}; // Create a buffer to store the result.
        if ( ::SHGetPathFromIDList( pIDL, fullPath ) != 0 )
        {
            retval = fullPath; // Set the string value.
        }

        CoTaskMemFree( pIDL ); // free the item
    }


/*
      //bi.iImage = 0;

   strcpy( c, g_InitDir.c_str() );
   strcat( c, "/folder" );

   bi.pszDisplayName = c;

   // if (newui)
   // {	bi.ulFlags=BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
   // }
   // else
   // {	bi.ulFlags=BIF_RETURNONLYFSDIRS;
   // }


   // initial directory check
#if OLD_CDW
   char old_cwd[MAX_PATH];
   getcwd( old_cwd, MAX_PATH );
#endif

#if 0
   bool path_exist = false;
   if (initDir)
   {
      std::string test = initialDir;
      path_exist = true; // IsPath(test.c_str());
   }

   // init directory AUSWERTUNG
   if (path_exist)
   {
      g_InitDir = initialDir;
      g_InitDir = AbsolutePath(g_InitDir.c_str());
      strcpy( c, g_InitDir.c_str() );
   }
   else
   {
      c[0]='\0';
      g_InitDir=curWorkingDir;
   }
   bi.pszDisplayName=(LPSTR)g_InitDir.c_str();
#endif

   char tmpDisplayName[MAX_PATH];
   size_t n = std::min( size_t( MAX_PATH ), caption.size() );
   strncpy( tmpDisplayName, caption.c_str(), n );
   bi.pszDisplayName = tmpDisplayName;

   // DIALOG
   std::string retVal;
   ::OleInitialize( nullptr );
   LPITEMIDLIST pIDL = SHBrowseForFolder( &bi );

   // DIALOG RESULT
   if ( pIDL )
   {
      char buffer[_MAX_PATH] = {'\0'}; // Create a buffer to store the result.
      if ( ::SHGetPathFromIDList( pIDL, buffer ) != 0 )
      {
         retVal = buffer; // Set the string value.
      }

      CoTaskMemFree( pIDL ); // free the item
   }
   ::OleUninitialize();

#if OLD_CDW
   SetCurrentDirectory( old_cwd );
#endif

   g_Caption.clear();
*/

   return retval;
}



//===========================================================
//===========================================================
//===========================================================

//
// OpenFileDialog
//

//===========================================================
//===========================================================
//===========================================================




//===========================================================
// static
ULONG64 CALLBACK
BenniOpenFileW_Proc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
//===========================================================
{
    if (uiMsg == BFFM_INITIALIZED)
    {
        // lpData contains the default path as a string
        SendMessage(hDlg, BFFM_SETSELECTION, TRUE, lParam);

        // Move and Resize dialog:
        if (g_Rect.w > 0 || g_Rect.h > 0)
        {
            if (g_Rect.w <= 0)
            {
                g_Rect.w = dbDesktopWidth() / 2;
                g_Rect.x = dbDesktopWidth() / 4;
            }

            if (g_Rect.h <= 0)
            {
                g_Rect.h = dbDesktopHeight() / 2;
                g_Rect.y = dbDesktopHeight() / 4;
            }

            //HWND hWnd = GetParent( hDlg );
            MoveWindow(hDlg, g_Rect.x, g_Rect.y, g_Rect.w, g_Rect.h, TRUE);
        }
    }
    return 0;

         // Position und Dimension
//			int cx=GetSystemMetrics(SM_CXSCREEN);
//			int cy=GetSystemMetrics(SM_CYSCREEN);

//         if (g_Rect.getWidth() <= 500) g_Rect.setWidth( 500 );
//         if (g_Rect.getHeight() <= 100 ) g_Rect.setHeight( 100 );
//         if (g_Rect.getWidth() > cx ) g_Rect.setWidth( cx );
//         if (g_Rect.getHeight() > cy ) g_Rect.setHeight( cy );
//         if (g_Rect.x1 < 0 ) g_Rect.x1 = (cx - g_Rect.getWidth())/2;
//         if (g_Rect.y1 < 0 ) g_Rect.y1 = (cy - g_Rect.getHeight())/2;
//         if (g_Rect.x1 > cx - g_Rect.getWidth() ) g_Rect.x1 = cx - g_Rect.getWidth();
//         if (g_Rect.y1 > cy - g_Rect.getHeight() ) g_Rect.y1 = cy - g_Rect.getHeight();

#if 0 // Adapt Dlg items in their size

    RECT r_win;
    GetClientRect( hWnd, &r_win );

    HWND btn_ro	 =	GetDlgItem( hWnd, 1040 );	// HIDE Read-only
    HWND btn_hp	 =	GetDlgItem( hWnd, 1038 );	// HIDE help
    HWND toolbar =	GetDlgItem( hWnd, 1184 );
    HWND listBox =	GetDlgItem( hWnd, 1120 );
    HWND label1  =	GetDlgItem( hWnd, 1090 );
    HWND edtBox	 =	GetDlgItem( hWnd, 1152 );
    HWND combo1  =	GetDlgItem( hWnd, 1148 );
    HWND label2	 =	GetDlgItem( hWnd, 1089 );
    HWND combo2  =	GetDlgItem( hWnd, 1136 );
    HWND btn_on	 =	GetDlgItem( hWnd, 1 );
    HWND btn_cn	 =	GetDlgItem( hWnd, 2 );

    SetWindowText( label1, "File Name" );
    SetWindowText( label2, "File Type" );

    // STYLE == 1 -> mit linker toolbar
    if ( g_bNewUI == true )
    {
        SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
        SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        int y=32;
        int x=100;
        SetWindowPos(toolbar,HWND_BOTTOM,5,             y,                x-10,                r_win.bottom-10,0);
        SetWindowPos(listBox,HWND_BOTTOM,x,             y,                r_win.right-x-5,     r_win.bottom-92,0);
        SetWindowPos(label1,HWND_BOTTOM,x,              r_win.bottom-53,	60,                  20,0);
        SetWindowPos(label2,HWND_BOTTOM,x,              r_win.bottom-28,	60,                  20,0);
        SetWindowPos(edtBox,HWND_BOTTOM,x+64,           r_win.bottom-55,	r_win.right-90-64-x,	20,0);
        SetWindowPos(combo1, HWND_BOTTOM,x+64,          r_win.bottom-55,	r_win.right-90-64-x,	150,0);
        SetWindowPos(combo2, HWND_BOTTOM,x+64,          r_win.bottom-30,	r_win.right-90-64-x,	150,0);
        SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,r_win.bottom-55,	65,                  20,0);
        SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,r_win.bottom-30,	65,                  20,0);
    }
    // STYLE == 0 -> KEINE linke toolbar
    else
    {
        SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
        SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        SetWindowPos(toolbar, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        int y=32;
        int x=5;
        SetWindowPos(listBox,HWND_BOTTOM,x,				y,				r_win.right-x-5,		r_win.bottom-92,0);
        SetWindowPos(label1,HWND_BOTTOM,x,				r_win.bottom-53,	60,					20,0);
        SetWindowPos(label2,HWND_BOTTOM,x,				r_win.bottom-28,	60,					20,0);
        SetWindowPos(edtBox,HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	20,0);
        SetWindowPos(combo1, HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	150,0);
        SetWindowPos(combo2, HWND_BOTTOM,x+64,			r_win.bottom-30,	r_win.right-90-64-x,	150,0);
        SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,	r_win.bottom-55,	65,					20,0);
        SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,	r_win.bottom-30,	65,					20,0);
    }
#endif // Adapt dialog items to user desired size.

    return 0;
}


//===========================================================
std::wstring
dbOpenFileDlg( de::OpenFileParamsW params )
//===========================================================
{
    params.initDir = de::FileSystem::makeWinPath(params.initDir);
    g_Rect = de::Recti( params.x, params.y, params.w, params.h );
    g_bNewUI = params.newui;

// #define OLD_CWD
// #ifdef OLD_CWD
//    // get current work directory
//    std::wstring old_cwd;
//    old_cwd.resize( size_t( MAX_PATH ), 0x00 );
//    GetCurrentDirectoryW( old_cwd.size(), old_cwd.data() );
//    //std::wcout << "GetCurrentDirectoryW() = " << old_cwd << std::endl;
// #endif

    // buffer for filename(s)
    wchar_t szFileName[65000];
    ZeroMemory( &szFileName, sizeof(szFileName) );
    wcscpy( szFileName, params.initFileName.c_str() );


    uint32_t flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
    //if (params.multiSelect) flags |= OFN_ALLOWMULTISELECT;

    OPENFILENAMEW dlg = { 0 };
    dlg.hwndOwner       = nullptr; //g_pGlob->hWnd;
    dlg.lStructSize     = sizeof(OPENFILENAMEW);
    dlg.lpstrTitle      = params.caption.c_str();
    dlg.lpstrFile       = szFileName;
    dlg.nMaxFile        = 65000;
    dlg.Flags           = flags;
    dlg.lpfnHook        = BenniOpenFileW_Proc;
    dlg.lpstrFilter     = nullptr; // params.filter.c_str();
    dlg.lpstrInitialDir = nullptr; // params.initDir.c_str();
    //dlg.hInstance			=	NULL;
    //dlg.lpstrCustomFilter	=	0;
    //dlg.nMaxCustFilter		=	0L;
    //dlg.nFilterIndex		=	1L;
    //dlg.nFileOffset		=	0;
    //dlg.nFileExtension		=	2;
    //dlg.lpstrDefExt		=	"*.*";
    //dlg.lCustData			=	0;
    //dlg.lpstrFileTitle		=	NULL;
    //dlg.nMaxFileTitle		=	0;

    std::wstring retVal;

    if ( GetOpenFileNameW( &dlg ) )
    {
        if ( dlg.lpstrFile )
        {
            retVal = dlg.lpstrFile;
        }
    }

    /*
   if ( GetOpenFileNameW( &dlg ) )
   {
      int len = wcslen( dlg.lpstrFile );

      if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
      {
         retVal = dlg.lpstrFile;
      }
      else
      {
         //wchar_t* data = nullptr;

         printf( "GetOpenFileNameW + multiple files Not impmenented\n" );
//         data = dlg.lpstrFile;  // This is the directory.
//         dlg.lpstrFile += len + 1;
//         while ( dlg.lpstrFile[0] )
//         {
//            //This will contain the current file name from multiple selection.
//            //FData = FData + "|" + ofn.lpstrFile;
//            vswprintf_s( data, "%s|%s", data, dlg.lpstrFile );

//            // Find next name
//            len = wcslen( dlg.lpstrFile );
//            dlg.lpstrFile += len + 1;
//         }
//         retVal = data;
      }
   }
   else
   {
      //retVal = "";
   }

#ifdef OLD_CWD
   SetCurrentDirectoryW( old_cwd.c_str() );
   //std::cout << "GetCurrentDirectoryA() = " << old_cwd << std::cout;
#endif
    */
    return retVal;
}


// UINT_PTR CALLBACK
// MyOFNHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam) {

//===========================================================
//static
ULONG64 CALLBACK
BenniOpenFileA_Proc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
//===========================================================
{
    if (uiMsg == BFFM_INITIALIZED)
    {
        // lpData contains the default path as a string
        SendMessage(hDlg, BFFM_SETSELECTION, TRUE, lParam);

        // Move and Resize dialog:
        if (g_Rect.w > 0 || g_Rect.h > 0)
        {
            if (g_Rect.w <= 0)
            {
                g_Rect.w = dbDesktopWidth() / 2;
                g_Rect.x = dbDesktopWidth() / 4;
            }

            if (g_Rect.h <= 0)
            {
                g_Rect.h = dbDesktopHeight() / 2;
                g_Rect.y = dbDesktopHeight() / 4;
            }

            //HWND hWnd = GetParent( hDlg );
            MoveWindow(hDlg, g_Rect.x, g_Rect.y, g_Rect.w, g_Rect.h, TRUE);
        }
    }

#if 0 // Adapt Dlg items in their size

    RECT r_win;
    GetClientRect( hWnd, &r_win );

    HWND btn_ro	 =	GetDlgItem( hWnd, 1040 );	// HIDE Read-only
    HWND btn_hp	 =	GetDlgItem( hWnd, 1038 );	// HIDE help
    HWND toolbar =	GetDlgItem( hWnd, 1184 );
    HWND listBox =	GetDlgItem( hWnd, 1120 );
    HWND label1  =	GetDlgItem( hWnd, 1090 );
    HWND edtBox	 =	GetDlgItem( hWnd, 1152 );
    HWND combo1  =	GetDlgItem( hWnd, 1148 );
    HWND label2	 =	GetDlgItem( hWnd, 1089 );
    HWND combo2  =	GetDlgItem( hWnd, 1136 );
    HWND btn_on	 =	GetDlgItem( hWnd, 1 );
    HWND btn_cn	 =	GetDlgItem( hWnd, 2 );

    SetWindowText( label1, "File Name" );
    SetWindowText( label2, "File Type" );

    // STYLE == 1 -> mit linker toolbar
    if ( g_bNewUI == true )
    {
        SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
        SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        int y=32;
        int x=100;
        SetWindowPos(toolbar,HWND_BOTTOM,5,             y,                x-10,                r_win.bottom-10,0);
        SetWindowPos(listBox,HWND_BOTTOM,x,             y,                r_win.right-x-5,     r_win.bottom-92,0);
        SetWindowPos(label1,HWND_BOTTOM,x,              r_win.bottom-53,	60,                  20,0);
        SetWindowPos(label2,HWND_BOTTOM,x,              r_win.bottom-28,	60,                  20,0);
        SetWindowPos(edtBox,HWND_BOTTOM,x+64,           r_win.bottom-55,	r_win.right-90-64-x,	20,0);
        SetWindowPos(combo1, HWND_BOTTOM,x+64,          r_win.bottom-55,	r_win.right-90-64-x,	150,0);
        SetWindowPos(combo2, HWND_BOTTOM,x+64,          r_win.bottom-30,	r_win.right-90-64-x,	150,0);
        SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,r_win.bottom-55,	65,                  20,0);
        SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,r_win.bottom-30,	65,                  20,0);
    }
    // STYLE == 0 -> KEINE linke toolbar
    else
    {
        SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
        SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        SetWindowPos(toolbar, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
        int y=32;
        int x=5;
        SetWindowPos(listBox,HWND_BOTTOM,x,				y,				r_win.right-x-5,		r_win.bottom-92,0);
        SetWindowPos(label1,HWND_BOTTOM,x,				r_win.bottom-53,	60,					20,0);
        SetWindowPos(label2,HWND_BOTTOM,x,				r_win.bottom-28,	60,					20,0);
        SetWindowPos(edtBox,HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	20,0);
        SetWindowPos(combo1, HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	150,0);
        SetWindowPos(combo2, HWND_BOTTOM,x+64,			r_win.bottom-30,	r_win.right-90-64-x,	150,0);
        SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,	r_win.bottom-55,	65,					20,0);
        SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,	r_win.bottom-30,	65,					20,0);
    }

   #endif // Adapt dialog items to user desired size.

    return 0;
}


//===========================================================
std::string
dbOpenFileDlg( de::OpenFileParamsA params )
//===========================================================
{
    params.initDir = de::FileSystem::makeWinPath( params.initDir );
    g_Rect = de::Recti( params.x, params.y, params.w, params.h );
    g_bNewUI = params.newui;

// #define OLD_CWD
// #ifdef OLD_CWD
//    // get current work directory
//    std::string old_cwd;
//    old_cwd.resize( size_t( MAX_PATH ), 0x00 );
//    GetCurrentDirectoryA( old_cwd.size(), old_cwd.data() );
//    DE_DEBUG("GetCurrentDirectoryA() = ", old_cwd)
// #endif

    // buffer for filename(s)
    char szFileName[64*1024];
    ZeroMemory( &szFileName, sizeof(szFileName) );

    // Set init filename
    if ( !params.initFileName.empty() )
    {
        strcpy( szFileName, params.initFileName.c_str() );
    }

    uint32_t flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
    // flags |= OFN_HIDEREADONLY
    // if (params.multiSelect) { flags |= OFN_ALLOWMULTISELECT; }

    OPENFILENAMEA dlg   = { 0 };
    dlg.lStructSize     = sizeof(OPENFILENAMEA);
    dlg.hwndOwner       = nullptr;
    dlg.lpstrTitle      = params.caption.c_str();
    dlg.lpstrFile       = szFileName;
    dlg.nMaxFile		= 64*1024;
    dlg.lpstrFilter     = params.filter.c_str();
    dlg.Flags           = flags;
    dlg.lpfnHook		= BenniOpenFileA_Proc;
    dlg.lpstrInitialDir = params.initDir.c_str();
    //dlg.hInstance			=	NULL;
    //dlg.lpstrCustomFilter	=	0;
    //dlg.nMaxCustFilter		=	0L;
    //dlg.nFilterIndex		=	1L;
    //dlg.nFileOffset		=	0;
    //dlg.nFileExtension		=	2;
    //dlg.lpstrDefExt		=	"*.*";
    //dlg.lCustData			=	0;
    //dlg.lpstrFileTitle		=	NULL;
    //dlg.nMaxFileTitle		=	0;

    // Set filetype filter
    // if ( .empty() )
    // {

    // }
    // else
    // {
    //     params.filter += "|";
    //     replace_to_null_char(params.filter);
    //     dlg.lpstrFilter = (LPSTR)params.filter.c_str();
    // }

    // DO MODAL DIALOG
    std::string retVal;
    // ::OleInitialize(NULL);

    if ( GetOpenFileNameA( &dlg ) )
    {
        if ( dlg.lpstrFile )
        {
            retVal = dlg.lpstrFile;
        }
    }
    /*

    if ( GetOpenFileNameA( &dlg ) )
    {
        char* data;
        int len = strlen( dlg.lpstrFile );

        if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
        {
            retVal = dlg.lpstrFile;
        }
        else
        {
            data = dlg.lpstrFile;  // This is the directory.
            dlg.lpstrFile += len + 1;
            while ( dlg.lpstrFile[0] )
            {
                //This will contain the current file name from multiple selection.
                //FData = FData + "|" + ofn.lpstrFile;
                sprintf( data, "%s|%s", data, dlg.lpstrFile );

                // Find next name
                len = strlen( dlg.lpstrFile );
                dlg.lpstrFile += len + 1;
            }
            retVal = data;
        }
    }
    */

// #ifdef OLD_CWD
//    SetCurrentDirectoryA( old_cwd.c_str() );
//    //std::cout << "GetCurrentDirectoryA() = " << old_cwd << std::cout;
// #endif

    return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
std::string
dbSaveFileDlg( de::SaveFileParamsA params )
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
    // bool multiSelect = false;
    params.initDir = de::FileSystem::makeWinPath( params.initDir );
    g_Rect = de::Recti( params.x, params.y, params.w, params.h );
    g_bNewUI = params.newui;

// #define OLD_CWD
// #ifdef OLD_CWD
//    // get current work directory
//    char old_cwd[MAX_PATH];
//    GetCurrentDirectoryA( MAX_PATH, old_cwd );
// #endif

    // buffer for filename(s)
    char szFileName[65000];
    ZeroMemory( &szFileName, sizeof(szFileName) );
    strcpy( szFileName, params.initFileName.c_str() );

    OPENFILENAMEA dlg = { 0 };
    // ZeroMemory( &dlg, sizeof(OPENFILENAMEA) );
    dlg.hwndOwner       = nullptr; //g_pGlob->hWnd;
    dlg.lStructSize     = sizeof(OPENFILENAMEA);
    dlg.lpstrTitle      = params.caption.c_str();
    dlg.lpstrFile       = szFileName;
    dlg.nMaxFile        = 65000;
    dlg.Flags           = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING; //  | OFN_HIDEREADONLY
    dlg.lpfnHook        = &BenniOpenFileA_Proc;
    dlg.lpstrFilter     = params.filter.c_str();
    dlg.lpstrInitialDir = params.initDir.c_str();
    //dlg.hInstance			=	NULL;
    //dlg.lpstrCustomFilter	=	0;
    //dlg.nMaxCustFilter		=	0L;
    //dlg.nFilterIndex		=	1L;
    //dlg.nFileOffset		=	0;
    //dlg.nFileExtension		=	2;
    //dlg.lpstrDefExt		=	"*.*";
    //dlg.lCustData			=	0;
    //dlg.lpstrFileTitle		=	NULL;
    //dlg.nMaxFileTitle		=	0;

    // DO MODAL DIALOG
    std::string retVal;

    if ( GetSaveFileNameA( &dlg ) )
    {
        if ( dlg.lpstrFile )	// If single file was selected.
        {
            retVal = dlg.lpstrFile;
        }
    }

    /*
    if ( GetSaveFileNameA( &dlg ) )
    {
        char* data;
        int len = strlen( dlg.lpstrFile );

        if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
        {
            retVal = dlg.lpstrFile;
        }
        else
        {
         data = dlg.lpstrFile;  // This is the directory.
         dlg.lpstrFile += len + 1;
         while ( dlg.lpstrFile[0] )
         {
            //This will contain the current file name from multiple selection.
            //FData = FData + "|" + ofn.lpstrFile;
            sprintf( data, "%s|%s", data, dlg.lpstrFile );

            // Find next name
            len = strlen( dlg.lpstrFile );
            dlg.lpstrFile += len + 1;
         }
         retVal = data;
    }


#ifdef OLD_CWD
   SetCurrentDirectoryA( old_cwd );
#endif

    */

    return retVal;
}


//===========================================================
static ULONG64 CALLBACK
BenniSaveFileW_Proc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
//===========================================================
{
    if (uiMsg == BFFM_INITIALIZED)
    {
        // lpData contains the default path as a string
        SendMessage(hDlg, BFFM_SETSELECTION, TRUE, lParam);

        // Move and Resize dialog:
        if (g_Rect.w > 0 || g_Rect.h > 0)
        {
            if (g_Rect.w <= 0)
            {
                g_Rect.w = dbDesktopWidth() / 2;
                g_Rect.x = dbDesktopWidth() / 4;
            }

            if (g_Rect.h <= 0)
            {
                g_Rect.h = dbDesktopHeight() / 2;
                g_Rect.y = dbDesktopHeight() / 4;
            }

            //HWND hWnd = GetParent( hDlg );
            MoveWindow(hDlg, g_Rect.x, g_Rect.y, g_Rect.w, g_Rect.h, TRUE);
        }
    }

    return 0;

   /*
   switch (uiMsg)
   {
      case WM_INITDIALOG:
      {
         // Position und Dimension
//			int cx=GetSystemMetrics(SM_CXSCREEN);
//			int cy=GetSystemMetrics(SM_CYSCREEN);

//         if (g_Rect.getWidth() <= 500) g_Rect.setWidth( 500 );
//         if (g_Rect.getHeight() <= 100 ) g_Rect.setHeight( 100 );
//         if (g_Rect.getWidth() > cx ) g_Rect.setWidth( cx );
//         if (g_Rect.getHeight() > cy ) g_Rect.setHeight( cy );
//         if (g_Rect.x1 < 0 ) g_Rect.x1 = (cx - g_Rect.getWidth())/2;
//         if (g_Rect.y1 < 0 ) g_Rect.y1 = (cy - g_Rect.getHeight())/2;
//         if (g_Rect.x1 > cx - g_Rect.getWidth() ) g_Rect.x1 = cx - g_Rect.getWidth();
//         if (g_Rect.y1 > cy - g_Rect.getHeight() ) g_Rect.y1 = cy - g_Rect.getHeight();

         HWND hWnd = GetParent( hDlg );
         MoveWindow( hWnd,
                     g_Rect.getX(),
                     g_Rect.getY(),
                     g_Rect.getWidth(),
                     g_Rect.getHeight(), true );

#if 1 // Adapt Dlg items in their size

         RECT r_win;
         GetClientRect( hWnd, &r_win );

         HWND btn_ro	 =	GetDlgItem( hWnd, 1040 );	// HIDE Read-only
         HWND btn_hp	 =	GetDlgItem( hWnd, 1038 );	// HIDE help
         HWND toolbar =	GetDlgItem( hWnd, 1184 );
         HWND listBox =	GetDlgItem( hWnd, 1120 );
         HWND label1  =	GetDlgItem( hWnd, 1090 );
         HWND edtBox	 =	GetDlgItem( hWnd, 1152 );
         HWND combo1  =	GetDlgItem( hWnd, 1148 );
         HWND label2	 =	GetDlgItem( hWnd, 1089 );
         HWND combo2  =	GetDlgItem( hWnd, 1136 );
         HWND btn_on	 =	GetDlgItem( hWnd, 1 );
         HWND btn_cn	 =	GetDlgItem( hWnd, 2 );

         SetWindowText( label1, "File Name" );
         SetWindowText( label2, "File Type" );

         // STYLE == 1 -> mit linker toolbar
         if ( g_bNewUI == true )
         {
            SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
            SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
            int y=32;
            int x=100;
            SetWindowPos(toolbar,HWND_BOTTOM,5,             y,                x-10,                r_win.bottom-10,0);
            SetWindowPos(listBox,HWND_BOTTOM,x,             y,                r_win.right-x-5,     r_win.bottom-92,0);
            SetWindowPos(label1,HWND_BOTTOM,x,              r_win.bottom-53,	60,                  20,0);
            SetWindowPos(label2,HWND_BOTTOM,x,              r_win.bottom-28,	60,                  20,0);
            SetWindowPos(edtBox,HWND_BOTTOM,x+64,           r_win.bottom-55,	r_win.right-90-64-x,	20,0);
            SetWindowPos(combo1, HWND_BOTTOM,x+64,          r_win.bottom-55,	r_win.right-90-64-x,	150,0);
            SetWindowPos(combo2, HWND_BOTTOM,x+64,          r_win.bottom-30,	r_win.right-90-64-x,	150,0);
            SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,r_win.bottom-55,	65,                  20,0);
            SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,r_win.bottom-30,	65,                  20,0);
            return 0;
         }

         // STYLE == 0 -> KEINE linke toolbar
         else
         {
            SetWindowPos(btn_ro,HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW); // HIDE Read Only
            SetWindowPos(btn_hp, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
            SetWindowPos(toolbar, HWND_BOTTOM,0,0,0,0,SWP_HIDEWINDOW);// HIDE Help
            int y=32;
            int x=5;
            SetWindowPos(listBox,HWND_BOTTOM,x,				y,				r_win.right-x-5,		r_win.bottom-92,0);
            SetWindowPos(label1,HWND_BOTTOM,x,				r_win.bottom-53,	60,					20,0);
            SetWindowPos(label2,HWND_BOTTOM,x,				r_win.bottom-28,	60,					20,0);
            SetWindowPos(edtBox,HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	20,0);
            SetWindowPos(combo1, HWND_BOTTOM,x+64,			r_win.bottom-55,	r_win.right-90-64-x,	150,0);
            SetWindowPos(combo2, HWND_BOTTOM,x+64,			r_win.bottom-30,	r_win.right-90-64-x,	150,0);
            SetWindowPos(btn_on, HWND_BOTTOM,r_win.right-85,	r_win.bottom-55,	65,					20,0);
            SetWindowPos(btn_cn, HWND_BOTTOM,r_win.right-85,	r_win.bottom-30,	65,					20,0);
            return 0;
         }
   #endif // Adapt dialog items to user desired size.

      }
   }
   */
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring
dbSaveFileDlg( de::SaveFileParamsW params )
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
    params.initDir = de::FileSystem::makeWinPath( params.initDir );
    g_Rect = de::Recti( params.x, params.y, params.w, params.h );
    g_bNewUI = params.newui;

// #define OLD_CWD
// #ifdef OLD_CWD
//    // get current work directory
//    std::wstring old_cwd;
//    old_cwd.resize( size_t( MAX_PATH ), 0x00 );
//    GetCurrentDirectoryW( old_cwd.size(), old_cwd.data() );
//    //std::wcout << "GetCurrentDirectoryW() = " << old_cwd << std::endl;
// #endif

    // buffer for filename(s)
    wchar_t szFileName[65000];
    ZeroMemory( &szFileName, sizeof(szFileName) );
    wcscpy( szFileName, params.initFileName.c_str() );

    OPENFILENAMEW dlg = { 0 };
    dlg.hwndOwner       = nullptr; //g_pGlob->hWnd;
    dlg.lStructSize     = sizeof(OPENFILENAMEW);
    dlg.lpstrTitle      = params.caption.c_str();
    dlg.lpstrFilter     = params.filter.c_str();
    dlg.Flags           = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING; //  | OFN_HIDEREADONLY
    dlg.nMaxFile        = 65000;
    dlg.lpstrFile       = szFileName;
    dlg.lpfnHook        = BenniSaveFileW_Proc;
    dlg.lpstrInitialDir = params.initDir.c_str();
    //dlg.nFilterIndex  = 1L;
    //dlg.hInstance			=	NULL;
    //dlg.lpstrCustomFilter	=	0;
    //dlg.nMaxCustFilter		=	0L;
    //dlg.nFileOffset		=	0;
    //dlg.nFileExtension		=	2;
    //dlg.lpstrDefExt		=	"*.*";
    //dlg.lCustData			=	0;
    //dlg.lpstrFileTitle		=	NULL;
    //dlg.nMaxFileTitle		=	0;

    std::wstring retVal;

    if ( GetSaveFileNameW( &dlg ) )
    {
        if ( dlg.lpstrFile )
        {
            retVal = dlg.lpstrFile;
        }
    }
    /*
   if ( GetSaveFileNameW( &dlg ) )
   {
      //char* data;
      int len = wcslen( dlg.lpstrFile );

      if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
      {
         retVal = dlg.lpstrFile;
      }
      else
      {
         printf( "GetSaveFileNameW + multiple files Not impmenented\n" );
//         data = dlg.lpstrFile;  // This is the directory.
//         dlg.lpstrFile += len + 1;
//         while ( dlg.lpstrFile[0] )
//         {
//            //This will contain the current file name from multiple selection.
//            //FData = FData + "|" + ofn.lpstrFile;
//            sprintf( data, "%s|%s", data, dlg.lpstrFile );

//            // Find next name
//            len = strlen( dlg.lpstrFile );
//            dlg.lpstrFile += len + 1;
//         }
//         retVal = data;
      }
   }
   else
   {
      printf( "GetSaveFileNameW had error\n" );
      retVal = L"";
   }

#ifdef OLD_CWD
   SetCurrentDirectoryW( old_cwd.c_str() );
#endif

    */

    return retVal;
}

/*

/////////////////////////////////////////////////////////////////////////////////////////////////////
string SaveDialog	( LPSTR lpCaption, int x, int y, int dx, int dy, bool newui, LPSTR filter, LPSTR initialDir, LPSTR lpFile, bool parent)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
   g_Rect.x=x;
   g_Rect.y=y;
   g_Rect.dx=dx;
   g_Rect.dy=dy;
   g_bNewUI=false;

   // get current work directory
   char curWorkingDir[MAX_PATH];
   getcwd(curWorkingDir,MAX_PATH);

   // buffer for filename
   char szFileName[MAX_PATH];
   ZeroMemory(&szFileName, sizeof(szFileName));

   OPENFILENAMEA my;
   ZeroMemory(&my, sizeof(OPENFILENAMEA));
   my.lStructSize			=	sizeof(OPENFILENAMEA); //OPENFILENAME_SIZE_VERSION_400;
   my.lpstrFile			=	szFileName;
   my.nMaxFile				=	65000;
   my.lpfnHook				=	&OFNHookProc;
   my.Flags				=	OFN_EXPLORER | OFN_ENABLEHOOK | OFN_HIDEREADONLY | OFN_ENABLESIZING;

   // FLAG :: Parent
   if (parent)
      my.hwndOwner			=	g_pGlob->hWnd;
   else
      my.hwndOwner			=	NULL; //g_pGlob->hWnd;

   // FLAG :: NewUI Style
   if (newui)
   {
      g_bNewUI=true;
   }
   else
   {
      g_bNewUI=false;
   }

   // initial filename
   if (lpFile)
   {
      string str_file = lpFile;
      strcpy(szFileName,str_file.c_str());
   }

   // filetype filter
   string str_filter;
   if (!filter)
   {
       my.lpstrFilter="All Files (*.*)\0*.*\0\0";
   }
   else
   {
       str_filter  = filter;
       str_filter += "|";
       replace_to_null_char(str_filter);
       my.lpstrFilter =	(LPSTR)str_filter.c_str();
   }

   // window caption
   string str_caption;
   if (!lpCaption)
   {
      my.lpstrTitle = NULL;
   }
   else
   {
      str_caption = lpCaption;
      replace_to_null_char(str_caption);
      my.lpstrTitle = (LPSTR)str_caption.c_str();
   }

   // initial directory
   string str_initdir;
   if (!initialDir)
   {
       str_initdir=curWorkingDir;
       my.lpstrInitialDir	=	NULL;
   }
   else
   {
      str_initdir = initialDir;
      //str_initdir += "|";
      replace_to_null_char(str_initdir);
      my.lpstrInitialDir = (LPSTR)str_initdir.c_str();
   }

   // DO MODAL DIALOG
   string retVal;

   if (GetSaveFileNameA(&my))
   {
      SetCurrentDirectory(curWorkingDir);
      retVal=my.lpstrFile;
   }
   else
   {
      retVal="";
   }
   return retVal;
}

////////////////////////////////////////////////////////////////////////////////////////////
void SetFont(HWND hwnd,LPTSTR FontName,int FontSize)
////////////////////////////////////////////////////////////////////////////////////////////
{
   HFONT hf;
   LOGFONT lf={0};
   HDC hdc=GetDC(hwnd);

   GetObject(GetWindowFont(hwnd),sizeof(lf),&lf);
   lf.lfWeight = FW_REGULAR;
   lf.lfHeight = (LONG)FontSize;
   lstrcpy( lf.lfFaceName, FontName );
   hf=CreateFontIndirect(&lf);
   SetBkMode(hdc,OPAQUE);
   SendMessage(hwnd,WM_SETFONT,(WPARAM)hf,TRUE);
   ReleaseDC(hwnd,hdc);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
UINT_PTR CALLBACK CCHookProc( HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
   if (uiMsg==WM_INITDIALOG)
   {
      //thcMessage("CC","WM_INITDIALOG");
      RECT Dialog;
      GetWindowRect(hdlg,&Dialog);

// Nur Position
      int cx=GetSystemMetrics(SM_CXSCREEN);
      int cy=GetSystemMetrics(SM_CYSCREEN);
      if (g_Rect.x<0)				  g_Rect.x=(cx-(Dialog.right-Dialog.left))/2;
      if (g_Rect.y<0)				  g_Rect.y=(cy-(Dialog.bottom-Dialog.top))/2;
      if (g_Rect.x>cx-(Dialog.right-Dialog.left)) g_Rect.x=cx-(Dialog.right-Dialog.left);
      if (g_Rect.y>cy-(Dialog.right-Dialog.left)) g_Rect.y=cy-(Dialog.right-Dialog.left);
      MoveWindow(hdlg,g_Rect.x,g_Rect.y,Dialog.right-Dialog.left,Dialog.bottom-Dialog.top,true);
      SetWindowText(hdlg,g_pCaption.c_str());
   }
   return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD ColorDialog( LPSTR lpCaption, int x, int y, DWORD c0, DWORD c1, DWORD c2, DWORD c3, DWORD c4, DWORD c5, DWORD c6, DWORD c7, DWORD c8, DWORD c9, DWORD cA, DWORD cB, DWORD cC, DWORD cD, DWORD cE, DWORD cF, bool parent)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
      //////////////////////////////////////////////////////////////////
      // size + position
      //////////////////////////////////////////////////////////////////
      g_Rect.x=x;
      g_Rect.y=y;
      g_pCaption=lpCaption;
      //////////////////////////////////////////////////////////////////
      // STRUCT BEGIN
      //////////////////////////////////////////////////////////////////
      DWORD *lpCustData = new DWORD[16];
      lpCustData[ 0]=c0;	lpCustData[ 1]=c1;	lpCustData[ 2]=c2;	lpCustData[ 3]=c3;
      lpCustData[ 4]=c4;	lpCustData[ 5]=c5;	lpCustData[ 6]=c6;	lpCustData[ 7]=c7;
      lpCustData[ 8]=c8;	lpCustData[ 9]=c9;	lpCustData[10]=cA;	lpCustData[11]=cB;
      lpCustData[12]=cC;	lpCustData[13]=cD;	lpCustData[14]=cE;	lpCustData[15]=cF;

      // Umwandlung von COLORREF (0x00bbggrr) in ARGB (heisst bei mir RGBA, obwohl es eigentlich nicht richtig ist) -> 0xaarrggbb
      for (int i=0; i<16; i++)
      {
         lpCustData[i] = RGB(RGBA_R(lpCustData[i]),RGBA_G(lpCustData[i]),RGBA_B(lpCustData[i]));
      }

      CHOOSECOLORA my;
      memset(&my, 0, sizeof(my));
      my.lStructSize=sizeof(my);
      my.hInstance=NULL;
      my.rgbResult=RGB(255,0,0);
      my.lpCustColors=lpCustData; // ptr
      my.Flags=CC_FULLOPEN | CC_ENABLEHOOK; // | CC_RGBINIT | CC_ANYCOLOR |
      my.lpfnHook=(LPCCHOOKPROC)CCHookProc;

      //////////////////////////////////////////////////////////////////
      // FLAG :: Parent
      //////////////////////////////////////////////////////////////////
      if (parent)
         my.hwndOwner			=	g_pGlob->hWnd;
      else
         my.hwndOwner			=	NULL; //g_pGlob->hWnd;

      //////////////////////////////////////////////////////////////////
      // DIALOG :: DO MODAL
      //////////////////////////////////////////////////////////////////
      DWORD back;
      if (ChooseColorA(&my))
      {
         //RGB(r,g,b) -> 0x00bbggrr == ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
         back=my.rgbResult;

         DWORD r=RGBA_B(back);
         DWORD g=RGBA_G(back);
         DWORD b=RGBA_R(back);
         back=RGBA(r,g,b,255);
      }
      else
      {
         back=RGBA(0,0,0,255);
      }
      delete lpCustData;
      return back;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
string FontDialog(	LPSTR lpCaption,	// dialog caption
               int x,				// dialog x-position
               int y,				// dialog y-position
               LPSTR lpFontname,	// fontname
               DWORD dwFontcolor,	// rgba
               int iFontsize,		// 1...1000
               int iStyle,			// 0 = normal, 1 = bold, 2=italic, 3 = bolditalic
               bool parent)
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
      g_Rect.x=x;
      g_Rect.y=y;
      g_pCaption=lpCaption;

      /////////////////////////////////////////////////////////////////////////////
      //
      // LOGFONT-STRUCT
      //
      /////////////////////////////////////////////////////////////////////////////
      LOGFONT LogFont;
      memset(&LogFont, 0, sizeof(LOGFONT));
      LogFont.lfWidth = 0;
      LogFont.lfEscapement = 0;
      LogFont.lfOrientation = 0;
      LogFont.lfUnderline = 0;
      LogFont.lfStrikeOut = 0;
      LogFont.lfCharSet = 0;
      LogFont.lfOutPrecision = OUT_STRING_PRECIS;
      LogFont.lfClipPrecision = CLIP_STROKE_PRECIS;
      LogFont.lfQuality = DEFAULT_QUALITY;
      LogFont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
      /////////////////////////////////////////////////////////////////////////////
      //
      // INITIAL FONT-FACE/NAME
      //
      /////////////////////////////////////////////////////////////////////////////
      if (lpFontname)
      {
         string face = lpFontname;
         strcpy(LogFont.lfFaceName, face.c_str());
      }
      else
      {
         strcpy(LogFont.lfFaceName, "Lucida Sans Unicode");
      }
      /////////////////////////////////////////////////////////////////////////////
      //
      // INITIAL FONT-SIZE
      //
      /////////////////////////////////////////////////////////////////////////////
      if (iFontsize<1)
         LogFont.lfHeight = -11; // size 8
      else
      {
         HDC hdc=GetDC(g_pGlob->hWnd);
         TEXTMETRIC tm;
         GetTextMetrics(hdc,&tm);
         LogFont.lfHeight = -MulDiv(iFontsize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
         ReleaseDC(g_pGlob->hWnd,hdc);
      }
      /////////////////////////////////////////////////////////////////////////////
      //
      // INITIAL FONT-WEIGHT
      //
      /////////////////////////////////////////////////////////////////////////////
      if ((iStyle==1) || (iStyle==3))
         LogFont.lfWeight = FW_BOLD;
      else
         LogFont.lfWeight = FW_NORMAL;
      /////////////////////////////////////////////////////////////////////////////
      //
      // INITIAL FONT-ITALIC
      //
      /////////////////////////////////////////////////////////////////////////////
      if ((iStyle==2) || (iStyle==3))
         LogFont.lfItalic = 1;
      else
         LogFont.lfItalic = 0;

      /////////////////////////////////////////////////////////////////////////////
      //
      // CHOOSEFONT - STRUCT (DIALOG)
      //
      /////////////////////////////////////////////////////////////////////////////
      CHOOSEFONT my;
      memset(&my, 0, sizeof(CHOOSEFONT));
      my.lStructSize=sizeof(CHOOSEFONT);
      my.Flags=CF_ENABLEHOOK | CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT ;
      my.hInstance=0;
      my.hDC=0;
      my.lpLogFont=&LogFont;
      my.lCustData=0;
      my.iPointSize=0;
      my.lpszStyle=0;
      my.lpfnHook=CCHookProc;
      my.lpTemplateName=NULL;
      my.nFontType=SCREEN_FONTTYPE;
      my.nSizeMin=0;
      my.nSizeMax=0;
      my.rgbColors=RGB(RGBA_R(dwFontcolor),RGBA_G(dwFontcolor),RGBA_B(dwFontcolor));

      //////////////////////////////////////////////////////////////////
      // FLAG :: Parent
      //////////////////////////////////////////////////////////////////
      if (parent)
         my.hwndOwner			=	g_pGlob->hWnd;
      else
         my.hwndOwner			=	NULL; //g_pGlob->hWnd;

      //////////////////////////////////////////////////////////////////
      // DIALOG :: DO MODAL
      //////////////////////////////////////////////////////////////////
      if (::ChooseFont(&my) != FALSE)
      {
      }
      else
      {
      }

      //--------------------------------------------------------------------------------------
      //	Formel für die Berechnung der Fontsize aus dpi in Pixel
      //--------------------------------------------------------------------------------------
      //	lfHeight = -MulDiv(PointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
      //
      //		                    (Height - Internal Leading) * 72
      //			Point Size  =    ---------------------------------
      //		                               LOGPIXELSY
      //
      //	Height - Cell height obtained from the TEXTMETRIC structure.
      //
      //	Internal Leading - Internal leading obtained from TEXTMETRIC structure.
      //
      //	72 - One point is 1/72 of an inch.
      //
      //	LOGPIXELSY - Number of pixels contained in a logical inch on the
      //					 device. This value can be obtained by calling the
      //					 GetDeviceCaps() function and specifying the LOGPIXELSY
      //					 index.

      // Get System Metrics
      HDC hdc=GetDC(g_pGlob->hWnd);
      TEXTMETRIC tm;
      GetTextMetrics(hdc,&tm);

      // Berechne Font Size
      int a = tm.tmInternalLeading;
      int b = LogFont.lfHeight;
      int c = GetDeviceCaps(hdc, LOGPIXELSY);
      iFontsize = ((a-b)*72/c)-2;
      ReleaseDC(g_pGlob->hWnd,hdc);

      //
      // BOLD
      //
      bool bBold;
      if (LogFont.lfWeight<700)
         bBold=0;
      else
         bBold=1;

      //
      // ITALIC
      //
      bool bItalic;
      if (LogFont.lfItalic>0)
         bItalic=1;
      else
         bItalic=0;
      //
      // STYLE
      //
      if ((bBold==0) && (bItalic==0)) iStyle=0;
      if ((bBold==1) && (bItalic==0)) iStyle=1;
      if ((bBold==0) && (bItalic==1)) iStyle=2;
      if ((bBold==1) && (bItalic==1)) iStyle=3;

   //
   // RETURN
   //
      DWORD dw_r=RGBA_B(my.rgbColors);
      DWORD dw_g=RGBA_G(my.rgbColors);
      DWORD dw_b=RGBA_R(my.rgbColors);
      stringstream s;
      s << LogFont.lfFaceName << "|";
      s << RGBA(dw_r,dw_g,dw_b,255) << "|";
      s << iFontsize << "|";
      s << iStyle;

   // RETURN string
   string retVal=s.str();
   return retVal;
}

*/

#endif
