#pragma once
#include <de/Core.h>

namespace de {

struct HKCU
{
    // ------------------------------------------------------------
    static bool
    existKey(const std::wstring& keyName);

    static bool
    existValue(const std::wstring& keyName,
               const std::wstring& valueName);

    // ------------------------------------------------------------
    static bool
    createKey(const std::wstring& keyName);

    static bool
    removeKey(const std::wstring& keyName, bool bRecursive);

    // ------------------------------------------------------------
    // Set REG_SZ value (value may not exist, will be created then)
    // ------------------------------------------------------------
    static bool
    writeREG_SZ(const std::wstring& keyName,
                const std::wstring& szName,
                const std::wstring& szValue);

    // ------------------------------------------------------------
    // Get REG_SZ, (Default) = L""
    // ------------------------------------------------------------
    static std::wstring
    readREG_SZ(const std::wstring& keyName,
               const std::wstring& valueName = L"");
};

} // end namespace de.
