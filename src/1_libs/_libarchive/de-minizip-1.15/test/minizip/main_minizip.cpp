//////////////////////////////////////////////////////////////////////////////
/// @file main_test.cpp
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#include <sstream>
#include <iostream>

#include <minizip/zip.h>

int main( int argc, char** argv )
{
   //   extern int zip_extract(const char *zipname, const char *dir,
   //                       int (*on_extract_entry)(const char *filename, void *arg),
   //                       void *arg);

   std::string exeFile = argv[0];

   if ( argc < 2 )
   {
      std::cout <<
      "[Usage] " << exeFile << "\n"
      "\n"
      "        $> minizip.exe                = Show this text\n"
      "        $> minizip.exe -h             = Show this text\n"
      "        $> minizip.exe -v --version   = Show version\n"
      "        $> minizip.exe -i             = Show interactive file dialog\n"
      "        $> minizip.exe <Archive.zip>  = Extract zip to new dir test1\n"
      "        $> minizip.exe <Archive.zip> <OutputDir> = Extract zip to new dir <OutputDir>\n"
      "\n"
      "        $> minizip.exe test1.zip         (extract zip to dir test1)\n"
      "        $> minizip.exe test1.zip C:/tmp  (extract zip to dir C:/tmp)\n"
      "\n"
      "        arg[0] Filename of *.zip archive to be decompressed\n"
      "        arg[1] Dirname of output-directory\n"
      "\n"
      "        Demo testing compression lib: #include <minizip/zip.h>\n"
      "\n"
      << std::endl;
   }
   else
   {
      std::string inFile;
      if ( argc > 1 )
      {
         inFile = argv[1];
      }

      std::string outDir;
      if ( argc > 2 )
      {
         outDir = argv[2];
      }
      if ( outDir.empty() )
      {
         outDir = "zip_temp_minizip";
      }

      std::cout << "SourceFile = " << inFile << std::endl;
      std::cout << "OutputDir = " << outDir << std::endl;
      int r = zip_extract( inFile.c_str(), outDir.c_str(), nullptr, nullptr );
      std::cout << "zip_extract => " << r << std::endl;
   }

   return 0;
}
