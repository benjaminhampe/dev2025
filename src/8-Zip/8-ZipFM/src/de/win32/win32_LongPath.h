#pragma once
#include <de/Core.h>

namespace de {

// Anticipate Long NT Paths (start with \\?\C: ...)
// std::wstring win32_getExeFileW();

// std::wstring win32_getExeDirW();

// std::string win32_getExeFileA();

// std::string win32_getExeDirA();

// std::wstring win32_to_nt_path(const std::wstring& p);

bool win32_canUseLongPaths();

bool win32_setRegistryLongPathAware( int enabled = 1 );

bool win32_isRegistryLongPathAware();

bool win32_isProcessLongPathAware();

bool win32_isFullyLongPathAware();

bool win32_isDriveAbsolute(const std::wstring& p);

bool win32_isUNC(const std::wstring& p);

bool win32_isAlreadyNT(const std::wstring& p);

std::wstring win32_toLongPath(const std::wstring& p);

} // end namespace de.
