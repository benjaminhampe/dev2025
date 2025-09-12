#include <clara.hpp>

namespace clara { 
namespace TextFlow {

bool isWhitespace( char c ) 
{
    static std::string chars = " \t\n\r";
    return chars.find( c ) != std::string::npos;
}

bool isBreakableBefore( char c )
{
    static std::string chars = "[({<|";
    return chars.find( c ) != std::string::npos;
}

bool isBreakableAfter( char c )
{
    static std::string chars = "])}>.,:;*+-=&/\\";
    return chars.find( c ) != std::string::npos;
}
    
} // end namespace TextFlow
} // end namespace clara