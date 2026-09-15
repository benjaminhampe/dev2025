#pragma once
#include <de/FileInfo.h>

namespace de {

void ScanDirectory_win32( FileInfos& fileInfos, std::wstring dir, bool recursive);

} // end namespace de.
