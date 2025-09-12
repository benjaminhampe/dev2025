#include <CPUInfo.hpp>
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

std::string DE_CC_DUMMY = "";
std::string DE_CC_RED = "\033[97;101m";
std::string DE_CC_RESET = "\033[0m";

//========================================================================
int main( int argc, char** argv )
//========================================================================
{
   std::cout << DE_CC_RED
      << "cpuid (c) 2022 by Benjamin Hampe <benjaminhampe@gmx.de>"
      << DE_CC_RESET  << std::endl;

   int cc = de_cpu_get_count();
   std::cout << DE_CC_DUMMY << "CPU count = " << cc << DE_CC_RESET << std::endl;

   float cs = de_cpu_get_speed_Mhz();
   std::cout << DE_CC_DUMMY << "CPU speed = " << cs << " MHz" << DE_CC_RESET << std::endl;

   CPUInfo info = dbCPUInfo();

   auto vs = info.vendorString;
   std::cout << DE_CC_DUMMY << "CPU vendor = " << vs << DE_CC_RESET  << std::endl;

   auto m = info.model;
   std::cout << DE_CC_DUMMY << "CPU model = " << m << DE_CC_RESET  << std::endl;

   int sc = info.socketCount;
   std::cout << DE_CC_DUMMY << "Socket count = " << sc << DE_CC_RESET  << std::endl;

   auto ds = info.dumpString;
   std::cout << DE_CC_DUMMY << "RegisterDump = \n" << ds << DE_CC_RESET  << std::endl;

   std::cout << DE_CC_DUMMY << "InfoDump = \n" << info.toString() << DE_CC_RESET  << std::endl;

   // int sc = info.socketCount;
   // std::cout << DE_CC_DUMMY << "Socket count = " << sc << DE_CC_RESET  << std::endl;


   std::cout << DE_CC_RED << "END" << DE_CC_RESET  << std::endl;
   return 0;
}

/*

//========================================================================
int main( int argc, char** argv )
//========================================================================
{
   std::string color_reset = "\033[0m";

   auto spaces = [] ( size_t n )
   {
      std::ostringstream s;
      for ( size_t i = 0; i < n; ++i ) { s << ' '; }
      return s.str();
   };

   std::cout <<
      "\n"
      "\033[97;41m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;43m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;42m" << spaces( 43 ) << "Hello World!"
                    << spaces( 43 ) << color_reset << "\n"
      "\033[97;46m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;44m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;45m" << spaces( 98 ) << color_reset << "\n"
      "\n"
      " Works with ANSI compatible terminals. "
      "If you see nothing than you probably use Windows cmd.exe.\n"
      "\n";

   uint8_t const* p = benni_rgb.data(); // rgb tupels

   for ( size_t y = 0; y < 68; ++y )
   {
      for ( size_t x = 0; x < 49; ++x )
      {
         int r = *p++;
         int g = *p++;
         int b = *p++;
         std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m  ";
      }
      std::cout << color_reset << "\n";
   }

   std::cout <<
      "\n\n\n"
      "\t\tC++ console with colors 2022 by Benjamin Hampe <benjaminhampe@gmx.de>\n"
      "\n\n\n"

      "\033[97;101m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;103m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;102m" << spaces( 98 ) << color_reset << "\n"
      "\033[90;106m" << spaces( 41 ) << "Have a nice day!"
                     << spaces( 41 ) << color_reset << "\n"
      "\033[97;104m" << spaces( 98 ) << color_reset << "\n"
      "\033[97;105m" << spaces( 98 ) << color_reset << "\n";

   return 0;
}

*/
