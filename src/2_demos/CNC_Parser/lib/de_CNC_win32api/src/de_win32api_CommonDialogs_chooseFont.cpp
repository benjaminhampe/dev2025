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
   //   RECT        g_Rect;
   //   bool        g_bNewUI;
   //   HWND        g_EditWindow;
   //   char        g_CharBuf[MAX_PATH];
} // end namespace anonymous


/////////////////////////////////////////////////////////////////////////////////////////////////////
std::string
dbChooseFontA( std::string caption, int x, int y,
               std::string fontName, u32 color, int fontSize, int style )
/////////////////////////////////////////////////////////////////////////////////////////////////////
{
   LOGFONTA initFont;
   memset(&initFont, 0, sizeof(LOGFONTA));
   initFont.lfWidth = 0;
   initFont.lfEscapement = 0;
   initFont.lfOrientation = 0;
   initFont.lfWeight = FW_NORMAL; // FW_BOLD;
   initFont.lfItalic = 0;
   initFont.lfUnderline = 0;
   initFont.lfStrikeOut = 0;
   initFont.lfCharSet = 0;
   initFont.lfOutPrecision = OUT_STRING_PRECIS;
   initFont.lfClipPrecision = CLIP_STROKE_PRECIS;
   initFont.lfQuality = DEFAULT_QUALITY;
   initFont.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;
   if ( fontName.size() > 0 )
   {
      size_t n = std::min( size_t(32), fontName.size() );
      strncpy( initFont.lfFaceName, fontName.c_str(), n); // "Lucida Sans Unicode"
   }
   /////////////////////////////////////////////////////////////////////////////
   //
   // INITIAL FONT-SIZE
   //
   /////////////////////////////////////////////////////////////////////////////
/*
      if (fontSize < 1)
         initFont.lfHeight = -11; // size 8
      else
      {
         HDC hdc = GetDC(g_pGlob->hWnd);
         TEXTMETRIC tm;
         GetTextMetrics(hdc,&tm);
         initFont.lfHeight = -MulDiv( fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72 );
         ReleaseDC(g_pGlob->hWnd,hdc);
      }
*/
   /////////////////////////////////////////////////////////////////////////////
   //
   // CHOOSEFONT - STRUCT (DIALOG)
   //
   /////////////////////////////////////////////////////////////////////////////
   CHOOSEFONTA my;
   memset(&my, 0, sizeof(CHOOSEFONTA));
   my.lStructSize = sizeof(CHOOSEFONTA);
   my.Flags       = CF_ENABLEHOOK | CF_EFFECTS | CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT ;
   my.hInstance   = nullptr;
   my.hDC         = nullptr;
   my.lpLogFont   = &initFont;
   my.lCustData   = 0;
   my.iPointSize  = 0;
   my.lpszStyle   = nullptr;
   my.lpfnHook    = nullptr; // CCHookProc;
   my.lpTemplateName = nullptr;
   my.nFontType   = SCREEN_FONTTYPE;
   my.nSizeMin    = 0;
   my.nSizeMax    = 0;
   my.rgbColors   = RGB( RGBA_R(color),RGBA_G(color),RGBA_B(color) );
   my.hwndOwner	= nullptr; //g_pGlob->hWnd;

   //////////////////////////////////////////////////////////////////
   // DIALOG :: DO MODAL
   //////////////////////////////////////////////////////////////////
   if ( ChooseFontA(&my) != FALSE )
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
   HDC hdc = GetDC( nullptr ); // g_pGlob->hWnd
   TEXTMETRIC tm;
   GetTextMetrics(hdc,&tm);

   // Berechne Font Size
   int a = tm.tmInternalLeading;
   int b = initFont.lfHeight;
   int c = GetDeviceCaps(hdc, LOGPIXELSY);
   fontSize = ((a-b)*72/c)-2;
   ReleaseDC( nullptr,hdc );

   //
   // BOLD
   //
   bool bBold;
   if (initFont.lfWeight<700)
      bBold=0;
   else
      bBold=1;

   //
   // ITALIC
   //
   bool bItalic;
   if (initFont.lfItalic>0)
      bItalic=1;
   else
      bItalic=0;
   //
   // STYLE
   //
   if ((bBold==0) && (bItalic==0)) style=0;
   if ((bBold==1) && (bItalic==0)) style=1;
   if ((bBold==0) && (bItalic==1)) style=2;
   if ((bBold==1) && (bItalic==1)) style=3;

   //
   // RETURN
   //
   u8 cr = RGBA_B(my.rgbColors);
   u8 cg = RGBA_G(my.rgbColors);
   u8 cb = RGBA_R(my.rgbColors);
   std::stringstream s;
   s << initFont.lfFaceName << "|" << RGBA(cr,cg,cb,255) << "|" << fontSize << "|" << style;

   return s.str();
}

/*
 *
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
std::string FontDialog(	LPSTR lpCaption,	// dialog caption
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
