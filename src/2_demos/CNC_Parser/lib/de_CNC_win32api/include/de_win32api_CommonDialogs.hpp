//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

std::string    dbGetDirA();
std::wstring   dbGetDirW();

void           dbSetDirA( std::string const & dir );
void           dbSetDirW( std::wstring const & dir );

void           dbMessageA( std::string const & msg, std::string const & caption = "MessageBoxA" );
void           dbMessageW( std::wstring const & msg, std::wstring const & caption = L"MessageBoxW" );

std::string    dbPromt( std::string const & msg, std::string const & caption = "Question" );

std::string
dbChooseDirA(  std::string caption = "Choose directory dialog <benjaminhampe@gmx.de>",
               std::string initDir = "",
               bool newui = true,
               int w = -1,
               int h = -1,
               int x = -1,
               int y = -1);

//std::string
//dbChooseDirW(  std::wstring caption = L"Choose directory dialog <benjaminhampe@gmx.de>",
//               std::wstring initDir = L"",
//               bool newui = true,
//               int w = -1,
//               int h = -1,
//               int x = -1,
//               int y = -1);

std::string
dbOpenFileA(   std::string caption = "Open file dialog <benjaminhampe@gmx.de>",
               std::string filter = "All files (*.*)|*.*",   // file filter
               std::string initUri = "",  // preselect dir or file, if ever.
               bool newui = true,
               int w = -1,
               int h = -1,
               int x = -1,
               int y = -1);

std::string
dbSaveFileA(   std::string caption = "Save file dialog <benjaminhampe@gmx.de>",
               std::string filter = "All files (*.*)|*.*",
               std::string initUri = "Untitled.txt",
               bool newui = true,
               int w = -1,
               int h = -1,
               int x = -1,
               int y = -1);

std::wstring
dbOpenFileW(   std::wstring caption = L"Open file dialog <benjaminhampe@gmx.de>",
               std::wstring filter = L"All files (*.*)|*.*",
               std::wstring initDir = L"",
               bool newui = true,
               int w = -1,
               int h = -1,
               int x = -1,
               int y = -1 );


std::wstring
dbSaveFileW( std::wstring caption = L"Save file dialog <benjaminhampe@gmx.de>",
             std::wstring filter = L"All files (*.*)|*.*",
             std::wstring initUri = L"Untitled",
             bool newui = true,
             int w = -1,
             int h = -1,
             int x = -1,
             int y = -1 );

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

inline u32 RGBA( u8 r, u8 g, u8 b, u8 a = 255)
{
   return (u32(a) << 24) + (u32(r) << 16) + (u32(g) << 8) + u32(b);
}
inline u8 RGBA_B(u32 color) { return color & 0xFF; }
inline u8 RGBA_G(u32 color) { return (color >> 8) & 0xFF; }
inline u8 RGBA_R(u32 color) { return (color >> 16) & 0xFF; }
inline u8 RGBA_A(u32 color) { return (color >> 24) & 0xFF; }

u32
dbChooseColor( std::string caption = "Choose color dialog <benjaminhampe@gmx.de>",
   int x = -1, int y = -1,
   u32 c0 = 0xFFFFFFFF,
   u32 c1 = 0xFFFFFFFF,
   u32 c2 = 0xFFFFFFFF,
   u32 c3 = 0xFFFFFFFF,
   u32 c4 = 0xFFFFFFFF,
   u32 c5 = 0xFFFFFFFF,
   u32 c6 = 0xFFFFFFFF,
   u32 c7 = 0xFFFFFFFF,
   u32 c8 = 0xFFFFFFFF,
   u32 c9 = 0xFFFFFFFF,
   u32 cA = 0xFFFFFFFF,
   u32 cB = 0xFFFFFFFF,
   u32 cC = 0xFFFFFFFF,
   u32 cD = 0xFFFFFFFF,
   u32 cE = 0xFFFFFFFF,
   u32 cF = 0xFFFFFFFF);

std::string
dbChooseFontA( std::string caption = "Choose font dialog <benjaminhampe@gmx.de>",
               int x = -1, int y = -1,
               std::string fontName = "Courier New",
               u32 color = 0xFF00FF00,
               int fontSize = 0,
               int style = 0 );

/*



namespace {
//namespace de {
//namespace window {

   // struct RECTi { int x, y, w, h; };
   // u32 RGBA(int r, int g, int b, int a);
   // int RGBA_R(u32 color);
   // int RGBA_G(u32 color);
   // int RGBA_B(u32 color);
   // int RGBA_A(u32 color);
   //inline void GetCurrentPath(char* buffer)
   //{
   //	getcwd(buffer, _MAX_PATH);
   //}

   std::string g_Caption;
   std::string g_InitDir;
   //de::Recti   g_Rect;
   RECT        g_Rect;
   bool        g_bNewUI;

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

//} // end namespace window
//} // end namespace de

} // end namespace anonymous

*/
