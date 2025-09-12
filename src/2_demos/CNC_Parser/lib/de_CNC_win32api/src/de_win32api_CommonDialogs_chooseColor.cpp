#include <de_win32api_CommonDialogs.hpp>
#include <de_win32api.hpp>

namespace {

   // struct RECTi { int x, y, w, h; };
   //inline void GetCurrentPath(char* buffer)
   //{
   //	getcwd(buffer, _MAX_PATH);
   //}

   std::string g_Caption;
   std::string g_InitDir;
   RECT        g_Rect;
   bool        g_bNewUI;
   HWND        g_EditWindow;
   char        g_CharBuf[MAX_PATH];

} // end namespace anonymous

u32
dbChooseColor( std::string caption, int x, int y,
   u32 c0, u32 c1, u32 c2, u32 c3,
   u32 c4, u32 c5, u32 c6, u32 c7,
   u32 c8, u32 c9, u32 cA, u32 cB,
   u32 cC, u32 cD, u32 cE, u32 cF )
{
   std::vector< u32 > initColors(16);
   initColors[ 0]=c0;	initColors[ 1]=c1;	initColors[ 2]=c2;	initColors[ 3]=c3;
   initColors[ 4]=c4;	initColors[ 5]=c5;	initColors[ 6]=c6;	initColors[ 7]=c7;
   initColors[ 8]=c8;	initColors[ 9]=c9;	initColors[10]=cA;	initColors[11]=cB;
   initColors[12]=cC;	initColors[13]=cD;	initColors[14]=cE;	initColors[15]=cF;

   // Umwandlung von COLORREF (0x00bbggrr) in ARGB (heisst bei mir RGBA, obwohl es eigentlich nicht richtig ist) -> 0xaarrggbb
   // for (int i=0; i<16; i++)
   // {
      // initColors[i] = RGB(RGBA_R(lpCustData[i]),RGBA_G(lpCustData[i]),RGBA_B(lpCustData[i]));
   // }

   CHOOSECOLORA dlg;
   memset(&dlg, 0, sizeof(dlg));
   dlg.hwndOwner	   = nullptr; //g_pGlob->hWnd;
   dlg.lStructSize   = sizeof(CHOOSECOLORA);
   dlg.hInstance     = nullptr;
   dlg.rgbResult     = RGB(255,0,0);
   dlg.lpCustColors  = reinterpret_cast<COLORREF*>(initColors.data());
   dlg.Flags         = CC_FULLOPEN; // | CC_RGBINIT | CC_ANYCOLOR |
   dlg.lpfnHook      = nullptr;
   //dlg.Flags       |= CC_ENABLEHOOK;
   //dlg.lpfnHook    = (LPCCHOOKPROC)CCHookProc;

   // DIALOG :: DO MODAL
   u32 back = 0;
   if ( ChooseColorA( &dlg ) )
   {
      //RGB(r,g,b) -> 0x00bbggrr == ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
      back = dlg.rgbResult;

      u8 r = RGBA_B(back);
      u8 g = RGBA_G(back);
      u8 b = RGBA_R(back);
      back = RGBA(r,g,b,255);
   }
   else
   {
      back = RGBA(0,0,0,255);
   }

   return back;
}

#if 0
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
DWORD dbChooseColor( LPSTR lpCaption, int x, int y, DWORD c0, DWORD c1, DWORD c2, DWORD c3, DWORD c4, DWORD c5, DWORD c6, DWORD c7, DWORD c8, DWORD c9, DWORD cA, DWORD cB, DWORD cC, DWORD cD, DWORD cE, DWORD cF, bool parent)
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
   std::vector< DWORD > initColors(16);
   initColors[ 0]=c0;	initColors[ 1]=c1;	initColors[ 2]=c2;	initColors[ 3]=c3;
   initColors[ 4]=c4;	initColors[ 5]=c5;	initColors[ 6]=c6;	initColors[ 7]=c7;
   initColors[ 8]=c8;	initColors[ 9]=c9;	initColors[10]=cA;	initColors[11]=cB;
   initColors[12]=cC;	initColors[13]=cD;	initColors[14]=cE;	initColors[15]=cF;

   // Umwandlung von COLORREF (0x00bbggrr) in ARGB (heisst bei mir RGBA, obwohl es eigentlich nicht richtig ist) -> 0xaarrggbb
   // for (int i=0; i<16; i++)
   // {
      // initColors[i] = RGB(RGBA_R(lpCustData[i]),RGBA_G(lpCustData[i]),RGBA_B(lpCustData[i]));
   // }

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

#endif
