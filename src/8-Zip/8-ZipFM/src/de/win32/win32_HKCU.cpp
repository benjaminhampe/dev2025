#include <de/win32/win32_HKCU.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
#endif

namespace de {

struct HKCU_Util
{
    static std::string errorStr(uint32_t errCode)
    {
        wchar_t* buffer = nullptr;

        DWORD size = FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errCode,
            0,
            (LPWSTR)&buffer,
            0,
            NULL
        );

        if (size == 0)
            return "Unknown error";

        std::wstring msg(buffer);
        LocalFree(buffer);

        return de_mbstr(msg);
    }

    static std::string keyStr(const std::wstring& subkey)
    {
        return "HKEY_CURRENT_USER\\" + de_mbstr(subkey);
    }

    /*
    // ------------------------------------------------------------
    // Convert registry value type to readable string
    // ------------------------------------------------------------
    static const char* getValueTypeStrA(uint32_t typ)
    {
        switch (typ)
        {
            case REG_NONE:        return "REG_NONE";
            case REG_SZ:          return "REG_SZ";
            case REG_EXPAND_SZ:   return "REG_EXPAND_SZ";
            case REG_BINARY:      return "REG_BINARY";
            case REG_DWORD:       return "REG_DWORD";
            case REG_DWORD_BIG_ENDIAN: return "REG_DWORD_BIG_ENDIAN";
            case REG_LINK:        return "REG_LINK";
            case REG_MULTI_SZ:    return "REG_MULTI_SZ";
            case REG_RESOURCE_LIST: return "REG_RESOURCE_LIST";
            case REG_FULL_RESOURCE_DESCRIPTOR: return "REG_FULL_RESOURCE_DESCRIPTOR";
            case REG_RESOURCE_REQUIREMENTS_LIST: return "REG_RESOURCE_REQUIREMENTS_LIST";
            case REG_QWORD:       return "REG_QWORD";
            default:              return "UNKNOWN";
        }
    }
    */
};

// ------------------------------------------------------------
// 1) Check if registry key exists
// ------------------------------------------------------------
// static
bool HKCU::existKey(const std::wstring& keyName)
{
    const HKEY root = HKEY_CURRENT_USER;
    HKEY hKey;
    auto hr = RegOpenKeyExW(root,
                keyName.c_str(),
                0,
                KEY_READ,
                &hKey);

    if (hr == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    DE_ERROR("errCode(",hr,"), ",
                HKCU_Util::errorStr(hr), ", ",
                HKCU_Util::keyStr(keyName))
    return false;
}

// ------------------------------------------------------------
// Check if value exists
// ------------------------------------------------------------
// static
bool HKCU::existValue(const std::wstring& keyName,
                       const std::wstring& valueName)
{
    const HKEY root = HKEY_CURRENT_USER;
    HKEY hKey;
    LONG hr = RegOpenKeyExW(root, keyName.c_str(), 0, KEY_READ, &hKey);
    if (hr == ERROR_SUCCESS)
    {
        hr = RegQueryValueExW(hKey, valueName.c_str(), NULL, NULL, NULL, NULL);
        RegCloseKey(hKey);

        if (hr == ERROR_SUCCESS)
        {
            return true;
        }
        else
        {
            DE_ERROR("(2.) errCode(",hr,"), ",
                HKCU_Util::errorStr(hr), ", ",
                HKCU_Util::keyStr(keyName))

            return false;
        }
    }
    else
    {
        DE_ERROR("(1.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return false;
    }
}

// ------------------------------------------------------------
// 2) Create registry key
// ------------------------------------------------------------
// static
bool HKCU::createKey(const std::wstring& keyName)
{
    HKEY hKey;
    DWORD disp;
    const HKEY root = HKEY_CURRENT_USER;
    LONG hr = RegCreateKeyExW(root, keyName.c_str(), 0, NULL, 0,
                             KEY_WRITE, NULL, &hKey, &disp);
    if (hr != ERROR_SUCCESS)
    {
        DE_ERROR("(1.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return false;
    }
    RegCloseKey(hKey);
    return true;
}

// ------------------------------------------------------------
// 3) Delete registry key (only if empty)
// ------------------------------------------------------------
// static
bool HKCU::removeKey(const std::wstring& keyName, bool bRecursive)
{
    if (!existKey(keyName))
    {
        return true;
    }

    const HKEY root = HKEY_CURRENT_USER;
    LONG hr = 0;
    if (bRecursive)
        hr = RegDeleteTreeW(root, keyName.c_str());
    else
        hr = RegDeleteKeyW(root, keyName.c_str());

    bool ok = (hr == ERROR_SUCCESS);
    if (!ok)
    {
        DE_ERROR("(1.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return false;
    }

    return ok;
}

// ------------------------------------------------------------
// 3) Set REG_SZ value
//    (Value muss NICHT existieren – wird automatisch angelegt)
// ------------------------------------------------------------
// static
bool HKCU::writeREG_SZ(const std::wstring& keyName,
                      const std::wstring& szName,
                      const std::wstring& szValue)
{
    const HKEY root = HKEY_CURRENT_USER;
    HKEY hKey;
    LONG hr = RegOpenKeyExW(root, keyName.c_str(), 0, KEY_SET_VALUE, &hKey);
    if (hr != ERROR_SUCCESS)
    {
        DE_ERROR("(1.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return false;
    }

    auto pSrc = reinterpret_cast<const BYTE*>(szValue.c_str());
    auto nSrc = static_cast<uint32_t>((szValue.size() + 1) * sizeof(wchar_t));

    if (szName.empty()) // (Default) value
    {
        hr = RegSetValueExW( hKey, nullptr, 0, REG_SZ, pSrc, nSrc);
    }
    else
    {
        hr = RegSetValueExW( hKey, szName.c_str(), 0, REG_SZ, pSrc, nSrc );
    }

    RegCloseKey(hKey);
    bool ok = (hr == ERROR_SUCCESS);
    if (!ok)
    {
        DE_ERROR("(2.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return false;
    }
    return ok;
}

// ------------------------------------------------------------
// Read REG_SZ
// ------------------------------------------------------------
// static
std::wstring
HKCU::readREG_SZ(const std::wstring& keyName,
                 const std::wstring& valueName)
{
    const HKEY root = HKEY_CURRENT_USER;

    HKEY hKey;
    DWORD hr = RegOpenKeyExW(root, keyName.c_str(), 0, KEY_READ, &hKey);
    if (hr != ERROR_SUCCESS)
    {
        DE_ERROR("(1.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return {};
    }

    // First query type + size
    DWORD type = 0;
    DWORD size = 0;
    hr = RegQueryValueExW(
        hKey,
        valueName.empty() ? nullptr : valueName.c_str(), // (Default) or other
        NULL,
        &type, // -> type: REG_SZ
        NULL,
        &size // -> size: Num wchars
    );

    if (hr != ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        DE_ERROR("(2.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return {}; // default value does not exist
    }

    // REG_SZ empty string = size == 2 bytes (L"\0")
    if (type == REG_SZ && size <= sizeof(wchar_t))
    {
        RegCloseKey(hKey);
        DE_ERROR("(3.) errCode(",hr,"), ",
            HKCU_Util::errorStr(hr), ", ",
            HKCU_Util::keyStr(keyName))
        return {}; // default value is empty
    }

    // If REG_SZ but size > 2, we need to check actual content
    if (type == REG_SZ)
    {
        std::vector<wchar_t> buffer(size / sizeof(wchar_t));

        hr = RegQueryValueExW(
            hKey,
            valueName.empty() ? nullptr : valueName.c_str(),
            NULL,
            NULL,
            (BYTE*)buffer.data(),
            &size
        );

        RegCloseKey(hKey);

        if (hr != ERROR_SUCCESS)
        {
            DE_ERROR("(4.) errCode(",hr,"), ",
                HKCU_Util::errorStr(hr), ", ",
                HKCU_Util::keyStr(keyName))
            return {};
        }

        return buffer.data();
    }

    // Other types (REG_DWORD, REG_BINARY, etc.)
    RegCloseKey(hKey);
    return {}; // No REG_SZ exists.
}

} // end namespace de.
