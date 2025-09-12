//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

std::string
dbOpenFileA(   std::string caption = "Open file dialog <benjaminhampe@gmx.de>",
               std::string filter = "All files (*.*)|*.*",   // file filter
               std::string initUri = "",  // preselect dir or file, if ever.
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