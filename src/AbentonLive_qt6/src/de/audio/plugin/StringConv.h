#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

struct StringConv
{
   static std::string
   toStr( std::wstring const & w );

   static std::wstring
   toWStr( std::string const & s );
};
