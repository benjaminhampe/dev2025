#include <de/Console.hpp>

namespace de {

Console::Console()
{}

/// @brief Write color reset marker.
std::string
Console::reset_colors() { return "\033[0m"; }

/// @brief Write color RGB marker. Foreground + Background colors.
std::string
Console::set_colors( uint8_t fr, uint8_t fg, uint8_t fb, uint8_t br, uint8_t bg, uint8_t bb )
{
   std::ostringstream o;
   o << "\033[38;2;" << int(fr) << ";" << int(fg) << ";" << int(fb) << "m";
   o << "\033[48;2;" << int(br) << ";" << int(bg) << ";" << int(bb) << "m";
   return o.str();
}


} // end namespace de

//   /// @brief Write a RGB color marker.
//   static std::ostream &
//   set_colors( std::ostream & o, uint32_t fg_color, uint32_t bg_color )
//   {
//      uint8_t fr = de::RGBA_R(fg_color);
//      uint8_t fg = de::RGBA_G(fg_color);
//      uint8_t fb = de::RGBA_B(fg_color);
//      o << "\033[38;2;" << fr << ";" << fg << ";" << fb << "m";

//      uint8_t br = de::RGBA_R(bg_color);
//      uint8_t bg = de::RGBA_G(bg_color);
//      uint8_t bb = de::RGBA_B(bg_color);
//      o << "\033[48;2;" << br << ";" << bg << ";" << bb << "m";
//      return o;
//   }

/*
   /// @brief Write a RGB color marker.
   static std::ostream &
   set_fg_color( std::ostream & o, int r, int g, int b )
   {
      o << "\033[38;2;" << r << ";" << g << ";" << b << "m";
      return o;
   }

   /// @brief Write a RGB color marker.
   static std::ostream &
   set_bg_color( std::ostream & o, int r, int g, int b )
   {
      o << "\033[48;2;" << r << ";" << g << ";" << b << "m";
      return o;
   }


   /// @brief Write reset marker.
   static std::ostream & reset( std::ostream & o ) { o << "\033[0m"; return o; }

   /// @brief Write a marker.
   static std::ostream & set_bold( std::ostream & o ) { o << "\033[1m"; return o; }
   static std::ostream & set_faint( std::ostream & o ) { o << "\033[2m"; return o; }
   static std::ostream & set_italic( std::ostream & o ) { o << "\033[3m"; return o; }
   static std::ostream & set_underline( std::ostream & o ) { o << "\033[4m"; return o; }
   static std::ostream & set_slowBlink( std::ostream & o ) { o << "\033[5m"; return o; }
   static std::ostream & set_rapidBlink( std::ostream & o ) { o << "\033[6m"; return o; }
   static std::ostream & set_encircled( std::ostream & o ) { o << "\033[52m"; return o; }
   static std::ostream & set_overlined( std::ostream & o ) { o << "\033[53m"; return o; }

// Name     FG  BG   - Name            FG  BG
// Black    30  40   - Bright Black    90  100
// Red      31  41   - Bright Red      91  101
// Green    32  42   - Bright Green    92  102
// Yellow   33  43   - Bright Yellow   93  103
// Blue     34  44   - Bright Blue     94  104
// Magenta  35  45   - Bright Magenta  95  105
// Cyan     36  46   - Bright Cyan     96  106
// White    37  47   - Bright White    97  107

enum EConsoleColor { Black = 0, Red, Green, Yellow, Blue, Magenta, Cyan, White, DefaultColor };

// Write a combined (foreground, background) color marker.
inline std::string
console_Color( EConsoleColor colorFG = Whit, bool brightFG = false,
               EConsoleColor colorBG = Blue, bool brightBG = false )
{
   int fg = 39; // ANSI: Set default foreground color
   int bg = 49; // ANSI: Set default background color
   if ( colorFG != DefaultColor )
   {
      fg = 30 + int( colorFG ); // 30 + 7 => fg = 37
      if ( brightFG ) fg += 60; // 37 + 60 => fg = 97
   }
   if ( colorBG != DefaultColor )
   {
      bg = 40 + int( colorBG );  // 40 + 4 => bg = 44
      if ( brightBG ) bg += 60;  // 44 + 60 => bg = 104
   }

//   std::string s = "\033["; // "\033[97;104m" -> BlueScreen
//   s += fg_color;
//   s += ';';
//   s += bg_color;
//   s += 'm';
   //return s;

   std::ostringstream s;
   s << "\033[" << fg << ';' << bg << 'm';

   return s.str(); // "\033[97;104m";
}


/// @brief Write specific combi color markers.
inline std::ostream & console_Ok( std::ostream & o ) { o << console_Color( White, true, Green, false ); return o; }
inline std::ostream & console_Warn( std::ostream & o ) { o << console_Color( Yellow, true, Yellow, false ); return o; }
inline std::ostream & console_Error( std::ostream & o ) { o << console_Color( White, true, Red, true ); return o; }
inline std::ostream & console_Debug( std::ostream & o ) { o << console_Color( White, true, Blue, true ); return o; }
inline std::ostream & console_Bluescreen(  std::ostream & o ) { o << console_Color( White, true, Blue, true ); return o; }


///// @brief Write specific foreground color markers (( std::cout << fg_white << "Hello World!\n" << color_reset.
//inline std::ostream & fg_white   ( std::ostream & o ) { o << fg( White, false ); return o; }
//inline std::ostream & fg_red     ( std::ostream & o ) { o << fg( Red, false ); return o; }
//inline std::ostream & fg_green   ( std::ostream & o ) { o << fg( Green, false ); return o; }
//inline std::ostream & fg_yellow  ( std::ostream & o ) { o << fg( Yellow, false ); return o; }
//inline std::ostream & fg_blue    ( std::ostream & o ) { o << fg( Blue, false ); return o; }
//inline std::ostream & fg_magenta ( std::ostream & o ) { o << fg( Magenta, false ); return o; }
//inline std::ostream & fg_cyan    ( std::ostream & o ) { o << fg( Cyan, false ); return o; }

//inline std::ostream & fg_white_bright  ( std::ostream & o ) { o << fg( White, true ); return o; }
//inline std::ostream & fg_red_bright    ( std::ostream & o ) { o << fg( Red, true ); return o; }
//inline std::ostream & fg_green_bright  ( std::ostream & o ) { o << fg( Green, true ); return o; }
//inline std::ostream & fg_yellow_bright ( std::ostream & o ) { o << fg( Yellow, true ); return o; }
//inline std::ostream & fg_blue_bright   ( std::ostream & o ) { o << fg( Blue, true ); return o; }
//inline std::ostream & fg_magenta_bright( std::ostream & o ) { o << fg( Magenta, true ); return o; }
//inline std::ostream & fg_cyan_bright   ( std::ostream & o ) { o << fg( Cyan, true ); return o; }

///// @brief Write specific background color markers.
//inline std::ostream & bg_white   ( std::ostream & o ) { o << bg( White, false ); return o; }
//inline std::ostream & bg_red     ( std::ostream & o ) { o << bg( Red, false ); return o; }
//inline std::ostream & bg_green   ( std::ostream & o ) { o << bg( Green, false ); return o; }
//inline std::ostream & bg_yellow  ( std::ostream & o ) { o << bg( Yellow, false ); return o; }
//inline std::ostream & bg_blue    ( std::ostream & o ) { o << bg( Blue, false ); return o; }
//inline std::ostream & bg_magenta ( std::ostream & o ) { o << bg( Magenta, false ); return o; }
//inline std::ostream & bg_cyan    ( std::ostream & o ) { o << bg( Cyan, false ); return o; }

//inline std::ostream & bg_white_bright  ( std::ostream & o ) { o << bg( White, true ); return o; }
//inline std::ostream & bg_red_bright    ( std::ostream & o ) { o << bg( Red, true ); return o; }
//inline std::ostream & bg_green_bright  ( std::ostream & o ) { o << bg( Green, true ); return o; }
//inline std::ostream & bg_yellow_bright ( std::ostream & o ) { o << bg( Yellow, true ); return o; }
//inline std::ostream & bg_blue_bright   ( std::ostream & o ) { o << bg( Blue, true ); return o; }
//inline std::ostream & bg_magenta_bright( std::ostream & o ) { o << bg( Magenta, true ); return o; }
//inline std::ostream & bg_cyan_bright   ( std::ostream & o ) { o << bg( Cyan, true ); return o; }

// Write a performance marker.
inline std::ostream &
perf( std::ostream & o )
{
   o << console_Bluescreen << '[' << std::this_thread::get_id() << "]" << console_Reset << ' ';
   return o;
}



// Name            FG  BG
// Black           30  40
// Red             31  41
// Green           32  42
// Yellow          33  43
// Blue            34  44
// Magenta         35  45
// Cyan            36  46
// White           37  47
// Bright Black    90  100
// Bright Red      91  101
// Bright Green    92  102
// Bright Yellow   93  103
// Bright Blue     94  104
// Bright Magenta  95  105
// Bright Cyan     96  106
// Bright White    97  107

enum EConsoleColor
{
   Black = 0, Red, Green, Yellow, Blue, Magenta, Cyan, White, DefaultColor
};

// Write a single (foreground) color marker to the console.
// 30 - Black, 31 - Red, 32 - Green, 33 - Yellow, 34 - Blue, 35 - Magenta, 36 - Cyan, 37 - White           37  47
// 90 - Bright Black, ..... , 97 - Bright White

inline std::string
fg( EConsoleColor consoleColor, bool bright = false )
{
   int colorFG = 39; // set default foreground color, use Sith order 39
   if ( consoleColor != DefaultColor ) // change Sith order 39 only if given color differs from DefaultColor.
   {
      colorFG = 30 + int( consoleColor ); // Use enum as index into foreground colors [30 ... 37]
      if ( bright ) colorFG += 60; // Use enum as index into bright foreground colors [90 ... 97]
   }
   std::string s = "\033[";
   s += colorFG;
   s += "m";
   return s;
}


// Write a single (foreground) color marker to the console.
// 40 - Black, 41 - Red, 42 - Green, 43 - Yellow, 44 - Blue, 45 - Magenta, 46 - Cyan, 47 - White           37  47
// 100 - Bright Black, ..... , 107 - Bright White

// Write a single (background) color marker to console.
inline std::string
bg( EConsoleColor consoleColor, bool bright = false )
{
   int colorBG = 49; // ANSI: Set default background color
   if ( consoleColor != DefaultColor )
   {
      colorBG = int( consoleColor ) + 40;
      if ( bright ) colorBG += 60;
   }

   std::string s = "\033[";
   s += colorBG;
   s += "m";
   return s;
}

/// @brief Write a color reset marker.
inline std::ostream &
reset_colors( std::ostream & o )
{
   o << "\033[0m";
   return o;
}

/// @brief Write specific foreground color markers (( std::cout << fg_white << "Hello World!\n" << color_reset.
inline std::ostream & fg_white   ( std::ostream & o ) { o << fg( White, false ); return o; }
inline std::ostream & fg_red     ( std::ostream & o ) { o << fg( Red, false ); return o; }
inline std::ostream & fg_green   ( std::ostream & o ) { o << fg( Green, false ); return o; }
inline std::ostream & fg_yellow  ( std::ostream & o ) { o << fg( Yellow, false ); return o; }
inline std::ostream & fg_blue    ( std::ostream & o ) { o << fg( Blue, false ); return o; }
inline std::ostream & fg_magenta ( std::ostream & o ) { o << fg( Magenta, false ); return o; }
inline std::ostream & fg_cyan    ( std::ostream & o ) { o << fg( Cyan, false ); return o; }

inline std::ostream & fg_white_bright  ( std::ostream & o ) { o << fg( White, true ); return o; }
inline std::ostream & fg_red_bright    ( std::ostream & o ) { o << fg( Red, true ); return o; }
inline std::ostream & fg_green_bright  ( std::ostream & o ) { o << fg( Green, true ); return o; }
inline std::ostream & fg_yellow_bright ( std::ostream & o ) { o << fg( Yellow, true ); return o; }
inline std::ostream & fg_blue_bright   ( std::ostream & o ) { o << fg( Blue, true ); return o; }
inline std::ostream & fg_magenta_bright( std::ostream & o ) { o << fg( Magenta, true ); return o; }
inline std::ostream & fg_cyan_bright   ( std::ostream & o ) { o << fg( Cyan, true ); return o; }

/// @brief Write specific background color markers.
inline std::ostream & bg_white   ( std::ostream & o ) { o << bg( White, false ); return o; }
inline std::ostream & bg_red     ( std::ostream & o ) { o << bg( Red, false ); return o; }
inline std::ostream & bg_green   ( std::ostream & o ) { o << bg( Green, false ); return o; }
inline std::ostream & bg_yellow  ( std::ostream & o ) { o << bg( Yellow, false ); return o; }
inline std::ostream & bg_blue    ( std::ostream & o ) { o << bg( Blue, false ); return o; }
inline std::ostream & bg_magenta ( std::ostream & o ) { o << bg( Magenta, false ); return o; }
inline std::ostream & bg_cyan    ( std::ostream & o ) { o << bg( Cyan, false ); return o; }

inline std::ostream & bg_white_bright  ( std::ostream & o ) { o << bg( White, true ); return o; }
inline std::ostream & bg_red_bright    ( std::ostream & o ) { o << bg( Red, true ); return o; }
inline std::ostream & bg_green_bright  ( std::ostream & o ) { o << bg( Green, true ); return o; }
inline std::ostream & bg_yellow_bright ( std::ostream & o ) { o << bg( Yellow, true ); return o; }
inline std::ostream & bg_blue_bright   ( std::ostream & o ) { o << bg( Blue, true ); return o; }
inline std::ostream & bg_magenta_bright( std::ostream & o ) { o << bg( Magenta, true ); return o; }
inline std::ostream & bg_cyan_bright   ( std::ostream & o ) { o << bg( Cyan, true ); return o; }

/// @brief Write specific combi color markers.
inline std::ostream & white_on_red( std::ostream & o ) { o << colors( White, true, Red, true ); return o; }
inline std::ostream & red_on_white( std::ostream & o ) { o << colors( White, true, Red, true ); return o; }
inline std::ostream & bluescreen(  std::ostream & o ) { o << colors( White, true, Blue, true ); return o; }

// Write a performance marker.
inline std::ostream &
perf( std::ostream & o )
{
   o << bluescreen
   << '['
   << std::this_thread::get_id()
   << "]"
   << reset_colors
   << ' ';
   return o;
}

*/
