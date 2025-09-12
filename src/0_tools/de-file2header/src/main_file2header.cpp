#include <de_core/de_file2header.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>

std::string getOpenFileNameA()
{
   char fn[MAX_PATH+1] {};
   OPENFILENAMEA ofn { sizeof(ofn) };
   ofn.lpstrFilter = "All Files(*.*)\0*.*\0\0";
   ofn.lpstrFile   = fn;
   ofn.nMaxFile    = _countof(fn);
   ofn.lpstrTitle  = "Select file to embedd as C/C++ header";
   ofn.Flags       = OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
   GetOpenFileNameA(&ofn);
   return fn;
}

//========================================================================
int main(int argc, char** argv)
//========================================================================
{
   std::string loadUri = getOpenFileNameA();

   if ( loadUri.empty() )
   {
      DG_ERROR("Got empty file, abort program")
      return 0;
   }

   std::string dataName = de::FileSystem::fileBase( loadUri );
   dataName = de::StringUtil::replace( dataName, "€", "Euro");
   dataName = de::StringUtil::replace( dataName, "@", "At");
   dataName = de::StringUtil::replace( dataName, " ", "_");
   dataName = de::StringUtil::replace( dataName, "(", "_");
   dataName = de::StringUtil::replace( dataName, ")", "_");
   dataName = de::StringUtil::replace( dataName, "[", "_");
   dataName = de::StringUtil::replace( dataName, "]", "_");
   dataName = de::StringUtil::replace( dataName, "<", "_");
   dataName = de::StringUtil::replace( dataName, ">", "_");
   dataName = de::StringUtil::replace( dataName, "{", "_");
   dataName = de::StringUtil::replace( dataName, "}", "_");
   dataName = de::StringUtil::replace( dataName, "|", "_");
   dataName = de::StringUtil::replace( dataName, ",", "_");
   dataName = de::StringUtil::replace( dataName, ";", "_");
   dataName = de::StringUtil::replace( dataName, ".", "_");
   dataName = de::StringUtil::replace( dataName, ":", "_");
   dataName = de::StringUtil::replace( dataName, "%", "_");
   dataName = de::StringUtil::replace( dataName, "Ä", "Ae");
   dataName = de::StringUtil::replace( dataName, "Ö", "Oe");
   dataName = de::StringUtil::replace( dataName, "Ü", "Ue");
   dataName = de::StringUtil::replace( dataName, "ß", "Sz");
   dataName = de::StringUtil::replace( dataName, "'", "_");
   dataName = de::StringUtil::replace( dataName, "\"", "_");
   dataName = de::StringUtil::replace( dataName, "!", "_");
   dataName = de::StringUtil::replace( dataName, "$", "_");
   dataName = de::StringUtil::replace( dataName, "&", "_");
   dataName = de::StringUtil::replace( dataName, "+", "_");
   dataName = de::StringUtil::replace( dataName, "#", "_");
   dataName = de::StringUtil::replace( dataName, "*", "_");
   dataName = de::StringUtil::replace( dataName, "-", "_");
   dataName = de::StringUtil::replace( dataName, "=", "_");
   dataName = de::StringUtil::replace( dataName, "`", "_");
   dataName = de::StringUtil::replace( dataName, "´", "_");
   dataName = de::StringUtil::replace( dataName, "^", "_");
   dataName = de::StringUtil::replace( dataName, "°", "_");

   de::FileToHeader::writeHeaderFromFile( dataName, loadUri );
   return 0;
}

