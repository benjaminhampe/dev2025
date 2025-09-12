/*
 *  FileSystem.cpp
 *
 *  Copyright (c) 2022-2023 Benjamin Hampe <benjaminhampe@gmx.de>
 *
 *  This file is free open source without any restriction or warranty.
 *
 */

#include <de_core/de_FileSystem.h>
#include <de_core/de_StringUtil.h>

// STL C++17
// #include <filesystem>
// namespace fs = std::filesystem;
// GCC 7.3.0 needs extra link with -lstdc++fs
//#include <experimental/filesystem> // GCC 7.3.0 and link with -lstdc++fs
#include <filesystem> // GCC 8+
#include <fstream>
//#include <iostream>

#include <de_core/de_PerformanceTimer.h>

namespace de {

} // end namespace de
