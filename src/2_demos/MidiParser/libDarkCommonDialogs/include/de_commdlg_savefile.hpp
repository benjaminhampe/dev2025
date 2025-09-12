//////////////////////////////////////////////////////////////////////////////
/// @author Benjamin Hampe <benjamin.hampe@gmx.de>
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <cstdint>
#include <sstream>
#include <vector>

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
dbSaveFileW( std::wstring caption = L"Save file dialog <benjaminhampe@gmx.de>",
             std::wstring filter = L"All files (*.*)|*.*",
             std::wstring initUri = L"Untitled",
             bool newui = true,
             int w = -1,
             int h = -1,
             int x = -1,
             int y = -1 );