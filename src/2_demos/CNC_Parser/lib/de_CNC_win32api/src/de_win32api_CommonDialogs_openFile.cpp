#include <de_win32api_CommonDialogs.hpp>
#include <de_win32api.hpp>
#include <iostream>

#ifndef USE_OLD_CWD
#define USE_OLD_CWD
#endif


namespace {

   //inline void GetCurrentPath(char* buffer)
   //{
   //	getcwd(buffer, _MAX_PATH);
   //}

   std::string g_Caption;
   std::string g_InitDir;
   RECT        g_Rect;
   bool        g_bNewUI;

   //===========================================================
   void replace_to_null_char( std::string & out )
   //===========================================================
   {
      for ( size_t i = 0; i < out.size(); ++i )
      {
         if ( out[i] == (unsigned char)124 )
              out[i] = 0x00;
      }
   }
} // end namespace anonymous


//===========================================================
std::string    dbOpenFileA(   std::string caption,
                              std::string filter,
                              std::string initDir,
                              bool newui, int w, int h, int x, int y )
//===========================================================
{
   std::string old_cwd = dbGetDirA();
   //OleInitialize(nullptr);

   // buffer for filename(s)
   char szFileName[65000];
   ZeroMemory( &szFileName, sizeof(szFileName) );
   strcpy( szFileName, "Untitled.txt" );

   OPENFILENAMEA dlg;
   ZeroMemory( &dlg, sizeof(OPENFILENAMEA) );
   //dlg.hInstance			=	NULL;
   dlg.lpstrTitle          = caption.c_str();
   dlg.hwndOwner           =	nullptr; //g_pGlob->hWnd;
   dlg.lStructSize			=	sizeof(OPENFILENAMEA);
   dlg.lpstrFile           =	szFileName;
   dlg.nMaxFile				=	65000;
   dlg.lpstrFilter         = "All Files (*.*)\0*.*\0\0";
   dlg.lpstrInitialDir     = nullptr; // (LPSTR)initDir.c_str();
   dlg.Flags               = OFN_EXPLORER
                           | OFN_FILEMUSTEXIST
                           | OFN_HIDEREADONLY
                           | OFN_ENABLESIZING; //  | OFN_ENABLE_HOOK | OFN_ALLOWMULTISELECT;
   dlg.lpfnHook				= nullptr;
   //dlg.lpstrCustomFilter	= 0;
   //dlg.nMaxCustFilter		= 0L;
   //dlg.nFilterIndex		= 1L;
   //dlg.nFileOffset       = 0;
   //dlg.nFileExtension		= 2;
   //dlg.lpstrDefExt       = "*.*";
   //dlg.lCustData			= 0;
   //dlg.lpstrFileTitle		= NULL;
   //dlg.nMaxFileTitle		= 0;

   std::string retVal;

   if ( GetOpenFileNameA( &dlg ) )
   {
      size_t len = strlen( dlg.lpstrFile );

      if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
      {
         retVal = dlg.lpstrFile;
      }
      else
      {
         char* data = dlg.lpstrFile;  // This is the directory.
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
   else
   {
      retVal = "";
   }

   dbSetDirA( old_cwd );
   //OleUninitialize();

   return retVal;
}


//===========================================================
std::wstring   dbOpenFileW(   std::wstring caption,
                              std::wstring filter,
                              std::wstring initDir,
                              bool newui, int w, int h, int x, int y )
//===========================================================
{
   //std::string old_cwd = dbGetDirA();
   //OleInitialize(nullptr);

   // buffer for filename(s)
   wchar_t szFileName[65000];
   ZeroMemory( &szFileName, sizeof(szFileName) );
   wcscpy( szFileName, L"Untitled.txt" );

   OPENFILENAMEW dlg;
   ZeroMemory( &dlg, sizeof(OPENFILENAMEW) );
   //dlg.hInstance			= nullptr;
   dlg.lpstrTitle          = caption.c_str();
   dlg.hwndOwner           = nullptr; //g_pGlob->hWnd;
   dlg.lStructSize			= sizeof(OPENFILENAMEW);
   dlg.lpstrFile           = szFileName;
   dlg.nMaxFile				= 65000;
   dlg.lpstrFilter         = L"All Files (*.*)\0*.*\0\0";
   dlg.lpstrInitialDir     = nullptr; // (LPSTR)initDir.c_str();
   dlg.Flags               = OFN_EXPLORER
                           | OFN_FILEMUSTEXIST
                           | OFN_HIDEREADONLY
                           | OFN_ENABLESIZING; //  | OFN_ENABLE_HOOK | OFN_ALLOWMULTISELECT;
   dlg.lpfnHook				= nullptr;
   //dlg.lpstrCustomFilter	= 0;
   //dlg.nMaxCustFilter		= 0L;
   //dlg.nFilterIndex		= 1L;
   //dlg.nFileOffset       = 0;
   //dlg.nFileExtension		= 2;
   //dlg.lpstrDefExt       = "*.*";
   //dlg.lCustData			= 0;
   //dlg.lpstrFileTitle		= NULL;
   //dlg.nMaxFileTitle		= 0;

   std::wstring retVal;

   if ( GetOpenFileNameW( &dlg ) )
   {
      size_t len = wcslen( dlg.lpstrFile );

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
      //retVal = L"";
   }
   //dbSetDirA( old_cwd );
   //OleUninitialize();

   return retVal;
}



/*
static ULONG64 CALLBACK
win32api_callback_OpenDialogFileA(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
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
                     g_Rect.left,
                     g_Rect.top,
                     g_Rect.right - g_Rect.left,
                     g_Rect.bottom - g_Rect.top, true );

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

         SetWindowTextA( label1, "File Name" );
         SetWindowTextA( label2, "File Type" );

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
   return 0;
}


//===========================================================
std::string    dbOpenFileA(   std::string caption,
                              std::string filter,
                              std::string initDir,
                              bool newui, int w, int h, int x, int y )
//===========================================================
{
   bool multiSelect = false;

   g_Rect.left = x;
   g_Rect.top = y;
   g_Rect.right = g_Rect.left + w;
   g_Rect.bottom = g_Rect.top + h;

   g_bNewUI = newui;

#ifdef USE_OLD_CWD
   std::string old_cwd = dbGetDirA();
#endif

   // buffer for filename(s)
   char szFileName[65000];
   ZeroMemory( &szFileName, sizeof(szFileName) );

   OPENFILENAMEA dlg;
   ZeroMemory( &dlg, sizeof(OPENFILENAMEA) );
   dlg.hwndOwner           =	nullptr; //g_pGlob->hWnd;
   dlg.lStructSize			=	sizeof(OPENFILENAMEA);
   dlg.lpstrFile           =	szFileName;
   dlg.nMaxFile				=	65000;
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

   dlg.lpfnHook				=  &win32api_callback_OpenDialogFileA;
   dlg.Flags=OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;

   // Set flag Multiline
   if (multiSelect)
      dlg.Flags |= OFN_ALLOWMULTISELECT;

   // Set init filename
//   if ( !initFileName.empty() )
//	{
      strcpy( dlg.lpstrFile, "Untitled" );
//	}

   // Set filetype filter
   if ( filter.empty() )
   {
       dlg.lpstrFilter = "All Files (*.*)\0*.*\0\0";
   }
   else
   {
       filter += "|";
       replace_to_null_char(filter);
       dlg.lpstrFilter = (LPSTR)filter.c_str();
   }

   // Set caption
   if ( caption.empty() )
   {
      dlg.lpstrTitle = "Benni's CommonDlg OpenFileNameA";
   }
   else
   {
      replace_to_null_char( caption );
      dlg.lpstrTitle = (LPSTR)caption.c_str();
   }

   // initial directory
   if ( initDir.empty() )
   {
       //dlg.lpstrInitialDir	= "";
   }
   else
   {
      replace_to_null_char( initDir );
      dlg.lpstrInitialDir = (LPSTR)initDir.c_str();
   }

   // DO MODAL DIALOG
   std::string retVal;
   OleInitialize(nullptr);

   if ( GetOpenFileNameA( &dlg ) )
   {
      size_t len = strlen( dlg.lpstrFile );

      if( dlg.lpstrFile[ len + 1 ] == 0 )	// If single file was selected.
      {
         retVal = dlg.lpstrFile;
      }
      else
      {
         char* data = dlg.lpstrFile;  // This is the directory.
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
   else
   {
      retVal = "";
   }

#ifdef USE_OLD_CWD
   dbSetDirA( old_cwd );
#endif

   OleUninitialize();

   return retVal;
}













//===========================================================
static ULONG64 CALLBACK
win32api_callback_OpenDialogFileW(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
//===========================================================
{
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
                     g_Rect.left,
                     g_Rect.top,
                     g_Rect.right - g_Rect.left,
                     g_Rect.bottom - g_Rect.top, true );

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

         SetWindowTextA( label1, "File Name" );
         SetWindowTextA( label2, "File Type" );

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
   return 0;
}


//===========================================================
std::wstring   dbOpenFileW(   std::wstring caption,
                              std::wstring filter,
                              std::wstring initDir,
                              bool newui, int w, int h, int x, int y )
//===========================================================
{
   bool multiSelect = false;

   g_Rect.left = x;
   g_Rect.top = y;
   g_Rect.right = g_Rect.left + w;
   g_Rect.bottom = g_Rect.top + h;
   g_bNewUI = newui;

#ifdef USE_OLD_CWD
   std::wstring old_cwd = dbGetDirW();
#endif

   // buffer for filename(s)
   wchar_t szFileName[65000];
   ZeroMemory( &szFileName, sizeof(szFileName) );

   OPENFILENAMEW dlg;
   ZeroMemory( &dlg, sizeof(OPENFILENAMEW) );
   dlg.hwndOwner           =	nullptr; //g_pGlob->hWnd;
   dlg.lStructSize			=	sizeof(OPENFILENAMEW);
   dlg.lpstrFile           =	szFileName;
   dlg.nMaxFile				=	65000;
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

   dlg.lpfnHook				=  &win32api_callback_OpenDialogFileW;
   dlg.Flags=OFN_EXPLORER | OFN_ENABLEHOOK | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING;

   // Set flag Multiline
   if (multiSelect)
      dlg.Flags |= OFN_ALLOWMULTISELECT;

   // Set init filename
//   if ( !initFileName.empty() )
//	{
      wcscpy( dlg.lpstrFile, L"Untitled" );
//	}

   // Set filetype filter
   if ( filter.empty() )
   {
      dlg.lpstrFilter = L"All Files (*.*)\0*.*\0\0";
      //wcscpy( dlg.lpstrFilter, L"All Files (*.*)" );
   }
   else
   {
//       filter += "|";
//       replace_to_null_char(filter);
       //wcscpy( dlg.lpstrFilter, filter.c_str() );
   }

   // Set caption
   if ( caption.empty() )
   {
      dlg.lpstrTitle = L"Benni's CommonDlg OpenFileNameW";
      //wcscpy( dlg.lpstrTitle, L"Benni's CommonDlg OpenFileNameW" );
   }
   else
   {
      //replace_to_null_char( caption );
      dlg.lpstrTitle = caption.c_str();
      //wcscpy( dlg.lpstrTitle, caption.c_str() );
   }

   // initial directory
   if ( initDir.empty() )
   {
      dlg.lpstrInitialDir = L"";
   }
   else
   {
      //replace_to_null_char( initDir );
      //wcscpy( dlg.lpstrInitialDir, initDir.c_str() );
      dlg.lpstrInitialDir = initDir.c_str();
   }

   // DO MODAL DIALOG
   std::wstring retVal;
   OleInitialize(NULL);

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

#ifdef USE_OLD_CWD
   dbSetDirW( old_cwd );
#endif

   OleUninitialize();

   return retVal;
}
*/
