//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

/*
std::string    dbGetDirA();
std::wstring   dbGetDirW();

void           dbSetDirA( std::string const & dir );
void           dbSetDirW( std::wstring const & dir );
*/

void           dbMessageA( std::string const & msg, std::string const & caption = "MessageBoxA" );
void           dbMessageW( std::wstring const & msg, std::wstring const & caption = L"MessageBoxW" );

/*
std::string    dbPromt( std::string const & msg, std::string const & caption = "Question" );
*/