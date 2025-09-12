#include <de_win32api_CommonDialogs.hpp>
#include <de_win32api.hpp>

//===========================================================
std::string
dbPromt( std::string const & msg, std::string const & caption )
{
   int result = MessageBoxA( nullptr, msg.c_str(), caption.c_str(), 4 );
   if ( result == 6 )
      return "true";
   else
      return "false";
}

//===========================================================
//bool
//dbPromtInt( int32_t & value, std::string const & msg, std::string const & caption )
//{
//   int result = MessageBoxA( nullptr, msg.c_str(), caption.c_str(), 4 );
//   if ( result == 6 )
//      return true;
//   else
//      return false;
//}

