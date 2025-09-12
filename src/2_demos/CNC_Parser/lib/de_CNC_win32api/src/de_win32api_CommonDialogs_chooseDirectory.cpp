#include <de_win32api_CommonDialogs.hpp>
#include <de_win32api.hpp>

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

//===========================================================
std::string dbChooseDirA(  std::string caption,
                           std::string initDir,
                           bool newui, int w, int h, int x, int y )
//===========================================================
{
   //g_Rect = de::Recti( x, y, w, h );
   //g_InitDir = initDir;

   BROWSEINFOA dlg;
   memset( &dlg, 0, sizeof( dlg ) );
   dlg.hwndOwner = nullptr; //g_pGlob->hWnd;
   dlg.lParam = reinterpret_cast<LPARAM>(&dlg);
   dlg.pidlRoot = nullptr;
   //dlg.iImage = 0;

   if ( caption.empty() )
   {
      caption = "Browse for Folder";
   }
   dlg.lpszTitle = caption.c_str();

   //   char c[MAX_PATH];
//   strcpy( c, initDir.c_str() );
//   strcat( c, "/folder" );
   std::string dir = initDir + "/folder";
   dlg.pszDisplayName = const_cast<LPSTR>(dir.c_str());

   dlg.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
   //if (!newui) {	dlg.ulFlags=BIF_RETURNONLYFSDIRS; }
   dlg.lpfn = nullptr; //CB_W32_OpenDirProcB;

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
   dlg.pszDisplayName=(LPSTR)g_InitDir.c_str();
#endif

   char tmpDisplayName[MAX_PATH];
   size_t n = std::min( size_t( MAX_PATH ), caption.size() );
   strncpy( tmpDisplayName, caption.c_str(), n );
   dlg.pszDisplayName = tmpDisplayName;

   // DIALOG
   std::string retVal;
   ::OleInitialize( nullptr );
   LPITEMIDLIST pIDL = SHBrowseForFolderA( &dlg );

   // DIALOG RESULT
   if ( pIDL )
   {
      char buffer[_MAX_PATH] = {'\0'}; // Create a buffer to store the result.
      if ( SHGetPathFromIDListA( pIDL, buffer ) != 0 )
      {
         retVal = buffer; // Set the string value.
      }

      CoTaskMemFree( pIDL ); // free the item
   }
   OleUninitialize();

#if OLD_CDW
   SetCurrentDirectory( old_cwd );
#endif

   //g_Caption.clear();

   return retVal;
}
