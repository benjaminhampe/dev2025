#pragma once
#include <de/FileInfo.h>
#include <de/FileInfoUtil.h>

namespace de {

void ScanDirectory( FileInfos& fileInfos, std::wstring dir, bool recursive);

} // end namespace de.
