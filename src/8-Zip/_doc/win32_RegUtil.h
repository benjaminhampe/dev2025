#pragma once
#include <cstdint>

#include <de/Core.h>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    // #define _WIN32_WINNT  0x0A00   // Windows 8 API freischalten
    // #define WINVER        0x0A00
    // #define _WIN32_WINNT  0x0602   // Windows 8 API freischalten
    // #define WINVER        0x0602
    #include <windows.h>            // only for Window ICOn
    #include <shellapi.h>
    //#include "../res/resource.h"    // only for Window ICOn
    // #include <shellscalingapi.h>    // Für SetProcessDpiAwarenessContext()
    // #include <winuser.h>
    // #include <dwmapi.h>

    // #pragma comment(lib, "Shlwapi.lib")
#endif


struct RegUtil
{
    static std::wstring getExePathW()
    {
        wchar_t buf[MAX_PATH];
        GetModuleFileNameW(nullptr, buf, MAX_PATH);
        return buf;
    }

    static std::string getExePathA()
    {
        return de_mbstr(getExePathW());
    }

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

    // ------------------------------------------------------------
    // Convert Win32 error code to readable message
    // ------------------------------------------------------------
    static std::wstring getErrorStrW(uint32_t errCode)
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
            return L"Unknown error";

        std::wstring msg(buffer);
        LocalFree(buffer);
        return msg;
    }

    static std::string getErrorStrA(uint32_t errCode)
    {
        return de_mbstr(getErrorStrW(errCode));
    }

    // ------------------------------------------------------------
    // Convert HKEY root handle to readable string
    // ------------------------------------------------------------
    static std::wstring getRootStrW(HKEY root)
    {
        if (root == HKEY_CLASSES_ROOT)      return L"HKEY_CLASSES_ROOT";
        if (root == HKEY_CURRENT_USER)      return L"HKEY_CURRENT_USER";
        if (root == HKEY_LOCAL_MACHINE)     return L"HKEY_LOCAL_MACHINE";
        if (root == HKEY_USERS)             return L"HKEY_USERS";
        if (root == HKEY_CURRENT_CONFIG)    return L"HKEY_CURRENT_CONFIG";
        if (root == HKEY_PERFORMANCE_DATA)  return L"HKEY_PERFORMANCE_DATA";
        if (root == HKEY_DYN_DATA)          return L"HKEY_DYN_DATA"; // legacy
        return L"UNKNOWN_ROOT";
    }

    static std::string getRootStrA(HKEY root)
    {
        return de_mbstr(getRootStrW(root));
    }

    // ------------------------------------------------------------
    //
    // ------------------------------------------------------------

    static std::wstring getStrW(HKEY root, const std::wstring& subkey)
    {
        return getRootStrW(root) + L"\\" + subkey;
    }

    static std::string getStrA(HKEY root, const std::wstring& subkey)
    {
        return getRootStrA(root) + "\\" + de_mbstr(subkey);
    }

    // ------------------------------------------------------------
    // 4) Recursive ASCII tree print using DE_DEBUG
    // ------------------------------------------------------------
    static void dump(HKEY root, const std::wstring& subkey,
                          const std::wstring& prefix = L"",
                          bool isLast = true)
    {
        HKEY hKey;
        auto r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return;
        }

        std::wstring line = prefix + (isLast ? L"└── " : L"├── ") + subkey;
        DE_DEBUG("REG ", de_mbstr(line))

        DWORD index = 0;
        wchar_t name[256*16];
        DWORD nameSize = sizeof(name);

        std::vector<std::wstring> children;

        while (RegEnumKeyExW(hKey, index, name, &nameSize,
                             NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            children.push_back(name);
            index++;
            nameSize = sizeof(name);
        }

        RegCloseKey(hKey);

        for (size_t i = 0; i < children.size(); ++i)
        {
            bool last = (i == children.size() - 1);
            std::wstring childPath = std::wstring(subkey) + L"\\" + children[i];

            dump(root, childPath.c_str(),
                      prefix + (isLast ? L"    " : L"│   "),
                      last);
        }
    }

    // ------------------------------------------------------------
    // 1) Check if registry key exists
    // ------------------------------------------------------------
    static bool existKey(HKEY root, const std::wstring& subkey)
    {
        HKEY hKey;
        LONG r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return true;
        }
        DE_ERROR("errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
        return false;
    }

    // ------------------------------------------------------------
    // Check if value exists
    // ------------------------------------------------------------
    static bool existValue(HKEY root,
                           const std::wstring& subkey,
                           const std::wstring& valueName)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r == ERROR_SUCCESS)
        {
            r = RegQueryValueExW(hKey, valueName.c_str(), NULL, NULL, NULL, NULL);
            RegCloseKey(hKey);

            if (r == ERROR_SUCCESS)
            {
                return true;
            }
            else
            {
                DE_ERROR("II. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
                return false;
            }
        }
        else
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return false;
        }
    }

    // ------------------------------------------------------------
    // 2) Create registry key
    // ------------------------------------------------------------
    static bool createKey(HKEY root, const std::wstring& subkey)
    {
        HKEY hKey;
        DWORD disp;
        LONG r = RegCreateKeyExW(root, subkey.c_str(), 0, NULL, 0,
                                 KEY_WRITE, NULL, &hKey, &disp);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return false;
        }
        RegCloseKey(hKey);
        return true;
    }

    // ------------------------------------------------------------
    // 3) Delete registry key (only if empty)
    // ------------------------------------------------------------
    static bool removeKey(HKEY root, const std::wstring& subkey, bool bRecursive)
    {
        if (!existKey(root,subkey))
        {
            DE_WARN("Key already deleted. ", getStrA(root,subkey))
            return true;
        }

        LONG r = 0;
        if (bRecursive)
            r = RegDeleteTreeW(root, subkey.c_str());
        else
            r = RegDeleteKeyW(root, subkey.c_str());

        bool ok = (r == ERROR_SUCCESS);
        if (!ok)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return false;
        }

        return ok;
    }


/*
    // ------------------------------------------------------------
    // Set DEFAULT value (REG_SZ)
    // ------------------------------------------------------------
    static DWORD SetDefaultString(HKEY root, const wchar_t* subkey,
                                  const wchar_t* value)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"SetDefaultString", r, subkey);
            return r;
        }

        // Default value = valueName = NULL
        r = RegSetValueExW(
            hKey,
            NULL, // default value
            0,
            REG_SZ,
            (const BYTE*)value,
            (DWORD)((wcslen(value) + 1) * sizeof(wchar_t))
        );

        RegCloseKey(hKey);

        Log(L"SetDefaultString", r, value);
        return r;
    }
*/

    // ------------------------------------------------------------
    // Check if DEFAULT value exists (valueName = NULL)
    // ------------------------------------------------------------
    static DWORD existDefaultValue(HKEY root, const std::wstring& subkey)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return r;
        }

        // Default value = valueName = NULL
        r = RegQueryValueExW(hKey, NULL, NULL, NULL, NULL, NULL);

        RegCloseKey(hKey);

        //Log(L"DefaultValueExists", r, L"(Default)");
        return r;
    }


        // ------------------------------------------------------------
    // Check if DEFAULT value exists and whether it is empty
    // outExists = true if value exists (even if empty)
    // outEmpty  = true if value exists but is empty ("")
    // ------------------------------------------------------------
    static std::optional<std::wstring> existValueREG_SZ( HKEY root, const std::wstring& subkey )
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return std::nullopt;
        }

        DWORD type = 0;
        DWORD size = 0;

        // First query: get type + size
        r = RegQueryValueExW(
            hKey,
            NULL,       // default value
            NULL,
            &type,
            NULL,
            &size
        );

        if (r != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            DE_ERROR("II. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return std::nullopt; // default value does not exist
        }

        // REG_SZ empty string = size == 2 bytes (L"\0")
        if (type == REG_SZ && size <= sizeof(wchar_t))
        {
            RegCloseKey(hKey);
            DE_ERROR("III. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return L""; // default value is empty
        }

        // If REG_SZ but size > 2, we need to check actual content
        if (type == REG_SZ)
        {
            std::vector<wchar_t> buffer(size / sizeof(wchar_t));

            DWORD r = RegQueryValueExW(
                hKey,
                NULL,
                NULL,
                NULL,
                (BYTE*)buffer.data(),
                &size
            );

            RegCloseKey(hKey);

            if (r != ERROR_SUCCESS)
            {
                DE_ERROR("IV. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
                return L"";
            }

            return buffer.data();
        }

        // Other types (REG_DWORD, REG_BINARY, etc.)
        RegCloseKey(hKey);
        return std::nullopt; // No REG_SZ exists.
    }

    // ✔ Function: Default value exists?


    // ------------------------------------------------------------
    // 3) Set REG_SZ value
    //    (Value muss NICHT existieren – wird automatisch angelegt)
    // ------------------------------------------------------------
    static bool writeREG_SZ(HKEY root, const std::wstring& subkey,
                          const std::wstring& stringName,
                          const std::wstring& stringValue)
    {
        HKEY hKey;
        LONG r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return false;
        }

        auto pSrc = reinterpret_cast<const BYTE*>(stringValue.c_str());
        auto nSrc = static_cast<uint32_t>((stringValue.size() + 1) * sizeof(wchar_t));

        if (stringName.empty()) // (Default) value
        {
            r = RegSetValueExW( hKey, nullptr,
                                0, REG_SZ, pSrc, nSrc);
        }
        else
        {
            r = RegSetValueExW( hKey, stringName.c_str(),
                                0, REG_SZ, pSrc, nSrc );
        }

        RegCloseKey(hKey);
        bool ok = (r == ERROR_SUCCESS);
        if (!ok)
        {
            DE_ERROR("II. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return false;
        }
        return ok;
    }

    // ------------------------------------------------------------
    // Read REG_SZ
    // ------------------------------------------------------------
    static std::wstring
    readREG_SZ(HKEY root,
                const std::wstring& subkey,
                const std::wstring& name = L"" /* (Default) */)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey.c_str(), 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS)
        {
            DE_ERROR("I. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return {};
        }

        // First query type + size
        DWORD type = 0;
        DWORD size = 0;
        r = RegQueryValueExW(
            hKey,
            name.empty() ? nullptr : name.c_str(), // (Default) or other
            NULL,
            &type, // -> type: REG_SZ
            NULL,
            &size // -> size: Num wchars
        );

        if (r != ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            DE_ERROR("II. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return {}; // default value does not exist
        }

        // REG_SZ empty string = size == 2 bytes (L"\0")
        if (type == REG_SZ && size <= sizeof(wchar_t))
        {
            RegCloseKey(hKey);
            DE_ERROR("III. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
            return {}; // default value is empty
        }

        // If REG_SZ but size > 2, we need to check actual content
        if (type == REG_SZ)
        {
            std::vector<wchar_t> buffer(size / sizeof(wchar_t));

            r = RegQueryValueExW(
                hKey,
                name.empty() ? nullptr : name.c_str(),
                NULL,
                NULL,
                (BYTE*)buffer.data(),
                &size
            );

            RegCloseKey(hKey);

            if (r != ERROR_SUCCESS)
            {
                DE_ERROR("IV. errCode(",r,") ",getErrorStrA(r), ", ",getStrA(root,subkey))
                return {};
            }

            return buffer.data();
        }

        // Other types (REG_DWORD, REG_BINARY, etc.)
        RegCloseKey(hKey);
        return {}; // No REG_SZ exists.
    }

/*
    // ------------------------------------------------------------
    // Read REG_SZ
    // ------------------------------------------------------------
    static DWORD readREG_SZ(HKEY root, const std::wstring& subkey,
                            const wchar_t* valueName,
                            std::wstring& outValue)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS) {
            //Log(L"ReadString", r, subkey);
            return r;
        }

        DWORD type = 0;
        DWORD size = 0;

        r = RegQueryValueExW(hKey, valueName, NULL, &type, NULL, &size);
        if (r != ERROR_SUCCESS || type != REG_SZ) {
            RegCloseKey(hKey);
            //Log(L"ReadString", r, valueName);
            return r;
        }

        std::vector<wchar_t> buffer(size / sizeof(wchar_t));

        r = RegQueryValueExW(hKey, valueName, NULL, NULL,
                             (BYTE*)buffer.data(), &size);

        RegCloseKey(hKey);

        if (r == ERROR_SUCCESS)
            outValue.assign(buffer.data());

        //Log(L"ReadString", r, valueName);
        return r;
    }

    // ------------------------------------------------------------
    // 5) Set REG_DWORD value
    // ------------------------------------------------------------
    static DWORD SetDWORD(HKEY root, const wchar_t* subkey,
                          const wchar_t* valueName, DWORD value)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"SetDWORD", r, subkey);
            return r;
        }

        r = RegSetValueExW(hKey, valueName, 0, REG_DWORD,
                           (const BYTE*)&value, sizeof(DWORD));

        RegCloseKey(hKey);

        Log(L"SetDWORD", r, valueName);
        return r;
    }


    // ------------------------------------------------------------
    // Read REG_DWORD
    // ------------------------------------------------------------
    static DWORD ReadDWORD(HKEY root, const wchar_t* subkey,
                           const wchar_t* valueName,
                           DWORD& outValue)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"ReadDWORD", r, subkey);
            return r;
        }

        DWORD type = 0;
        DWORD size = sizeof(DWORD);

        r = RegQueryValueExW(hKey, valueName, NULL, &type,
                             (BYTE*)&outValue, &size);

        RegCloseKey(hKey);

        if (r != ERROR_SUCCESS || type != REG_DWORD)
            return r;

        Log(L"ReadDWORD", r, valueName);
        return r;
    }


    // ------------------------------------------------------------
    // 6) Set REG_BINARY value
    // ------------------------------------------------------------
    static bool SetBinary(HKEY root, const wchar_t* subkey,
                          const wchar_t* valueName,
                          const BYTE* data, DWORD size)
    {
        HKEY hKey;
        LONG r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS)
            return false;

        r = RegSetValueExW(hKey, valueName, 0, REG_BINARY, data, size);

        RegCloseKey(hKey);
        return (r == ERROR_SUCCESS);
    }



    // ------------------------------------------------------------
    // Set REG_BINARY
    // ------------------------------------------------------------
    static DWORD SetBinary(HKEY root, const wchar_t* subkey,
                           const wchar_t* valueName,
                           const BYTE* data, DWORD size)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"SetBinary", r, subkey);
            return r;
        }

        r = RegSetValueExW(hKey, valueName, 0, REG_BINARY, data, size);

        RegCloseKey(hKey);

        Log(L"SetBinary", r, valueName);
        return r;
    }


    // ------------------------------------------------------------
    // Read REG_BINARY
    // ------------------------------------------------------------
    static DWORD ReadBinary(HKEY root, const wchar_t* subkey,
                            const wchar_t* valueName,
                            std::vector<BYTE>& outData)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"ReadBinary", r, subkey);
            return r;
        }

        DWORD type = 0;
        DWORD size = 0;

        r = RegQueryValueExW(hKey, valueName, NULL, &type, NULL, &size);
        if (r != ERROR_SUCCESS || type != REG_BINARY) {
            RegCloseKey(hKey);
            Log(L"ReadBinary", r, valueName);
            return r;
        }

        outData.resize(size);

        r = RegQueryValueExW(hKey, valueName, NULL, NULL,
                             outData.data(), &size);

        RegCloseKey(hKey);

        Log(L"ReadBinary", r, valueName);
        return r;
    }

    // ------------------------------------------------------------
    // 7) Set REG_MULTI_SZ value
    // ------------------------------------------------------------
    static bool SetMultiSZ(HKEY root, const wchar_t* subkey,
                           const wchar_t* valueName,
                           const std::vector<std::wstring>& strings)
    {
        HKEY hKey;
        LONG r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS)
            return false;

        // Build MULTI_SZ buffer
        std::vector<wchar_t> buffer;
        for (const auto& s : strings) {
            buffer.insert(buffer.end(), s.begin(), s.end());
            buffer.push_back(L'\0');
        }
        buffer.push_back(L'\0'); // final double-null terminator

        r = RegSetValueExW(hKey, valueName, 0, REG_MULTI_SZ,
                           (const BYTE*)buffer.data(),
                           (DWORD)(buffer.size() * sizeof(wchar_t)));

        RegCloseKey(hKey);
        return (r == ERROR_SUCCESS);
    }


    // ------------------------------------------------------------
    // Set REG_MULTI_SZ
    // ------------------------------------------------------------
    static DWORD SetMultiSZ(HKEY root, const wchar_t* subkey,
                            const wchar_t* valueName,
                            const std::vector<std::wstring>& strings)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_SET_VALUE, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"SetMultiSZ", r, subkey);
            return r;
        }

        std::vector<wchar_t> buffer;
        for (const auto& s : strings) {
            buffer.insert(buffer.end(), s.begin(), s.end());
            buffer.push_back(L'\0');
        }
        buffer.push_back(L'\0');

        r = RegSetValueExW(hKey, valueName, 0, REG_MULTI_SZ,
                           (const BYTE*)buffer.data(),
                           (DWORD)(buffer.size() * sizeof(wchar_t)));

        RegCloseKey(hKey);

        Log(L"SetMultiSZ", r, valueName);
        return r;
    }

    // ------------------------------------------------------------
    // Read REG_MULTI_SZ
    // ------------------------------------------------------------
    static DWORD ReadMultiSZ(HKEY root, const wchar_t* subkey,
                             const wchar_t* valueName,
                             std::vector<std::wstring>& outStrings)
    {
        HKEY hKey;
        DWORD r = RegOpenKeyExW(root, subkey, 0, KEY_READ, &hKey);
        if (r != ERROR_SUCCESS) {
            Log(L"ReadMultiSZ", r, subkey);
            return r;
        }

        DWORD type = 0;
        DWORD size = 0;

        r = RegQueryValueExW(hKey, valueName, NULL, &type, NULL, &size);
        if (r != ERROR_SUCCESS || type != REG_MULTI_SZ) {
            RegCloseKey(hKey);
            Log(L"ReadMultiSZ", r, valueName);
            return r;
        }

        std::vector<wchar_t> buffer(size / sizeof(wchar_t));

        r = RegQueryValueExW(hKey, valueName, NULL, NULL,
                             (BYTE*)buffer.data(), &size);

        RegCloseKey(hKey);

        if (r != ERROR_SUCCESS)
            return r;

        // Parse MULTI_SZ
        outStrings.clear();
        const wchar_t* ptr = buffer.data();

        while (*ptr != L'\0') {
            std::wstring s(ptr);
            outStrings.push_back(s);
            ptr += s.size() + 1;
        }

        Log(L"ReadMultiSZ", r, valueName);
        return r;
    }

*/




    static bool writeKeyValue(const std::wstring& key, const std::wstring& value)
    {
        HKEY hKey;
        auto ok = RegCreateKeyExW(HKEY_CURRENT_USER, key.c_str(),
                    0,
                    nullptr,
                    0,
                    KEY_WRITE,
                    nullptr,
                    &hKey,
                    nullptr
            );

        if (ok != ERROR_SUCCESS)
        {
            DE_ERROR("Fail RegCreateKeyExW(HKCU, key=",de_mbstr(key),")")
            return false;
        }

        LONG r = RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                                (BYTE*)value.c_str(),
                                (DWORD)((value.size() + 1) * sizeof(wchar_t)));
        RegCloseKey(hKey);
        return r == ERROR_SUCCESS;
    }

    static bool writeCommand(const std::wstring& key, const std::wstring& exePath)
    {
        std::wstring value = L"\"" + exePath + L"\" \"%1\"";
        return writeKeyValue(key,value);
    }

    static std::wstring readCommand(const wchar_t* key)
    {
        HKEY hKey;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return L"";

        wchar_t buf[1024];
        DWORD size = sizeof(buf);
        LONG r = RegQueryValueExW(hKey, nullptr, nullptr, nullptr,
                                  (BYTE*)buf, &size);
        RegCloseKey(hKey);

        if (r != ERROR_SUCCESS)
            return L"";

        return buf;
    }





};

/*
// 🧪 Beispiel: Deinen Ordner‑Kontextmenü‑Tree ausgeben

RegistryManager::PrintTree(
    HKEY_CURRENT_USER,
    "Software\\Classes\\Directory\\shell\\8-zip Compress"
);


EXE_NAME = "C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe";

🧩 Windows context menu entries come from three places:

    🧩 HKCR\*\shell — items for any file
    🧩 HKCR\Directory\shell — items for folders
    🧩 HKCR\AllFilesystemObjects\shell — items for both files and folders

    COM shell extensions — dynamic menus (for cascading full menu)

// ========================================================================
// [1] Files = [*]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\*\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\*\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ *
         └─ shell
            └─ 8-zip Compress
               ├─ (Standard)      REG_SZ 8-zip Compress
               ├─ Icon            REG_SZ "${EXE_NAME}"
               └─ command
                  └─ (Standard)   REG_SZ "${EXE_NAME}" --compress "%1"


// ========================================================================
// [2] Directories = [Directory]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ Directory
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --compress "%1"

// ========================================================================
// [3] Files + Directories = [AllFilesystemObjects]
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\AllFilesystemObjects\shell\8-zip Compress]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\AllFilesystemObjects\shell\8-zip Compress\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ AllFilesystemObjects
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --compress "%1"



// ========================================================================
// [4] Extract .tar Files
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\tar\shell\8-zip Extract]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\tar\shell\8-zip Extract\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ tar
         └─ shell
            └─ 8-zip Extract
               ├─ (Default)        REG_SZ   "8-zip Extract"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --extract "%1"

// ========================================================================
// [4] Extract .zst Files
// ========================================================================

[HKEY_CURRENT_USER\Software\Classes\zst\shell\8-zip Extract]
    @="8-zip Compress"
    "Icon"="${EXE_NAME}"

[HKEY_CURRENT_USER\Software\Classes\zst\shell\8-zip Extract\command]
    @="${EXE_NAME}" "%1"

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ zst
         └─ shell
            └─ 8-zip Extract
               ├─ (Default)        REG_SZ   "8-zip Extract"
               ├─ Icon             REG_SZ   "${EXE_NAME}"
               └─ command
                  └─ (Default)     REG_SZ   "${EXE_NAME}" --extract "%1"

#include <windows.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <iostream>

#pragma comment(lib, "Shlwapi.lib")

static const wchar_t* KEY_FILE =
    L"Software\\Classes\\*\\shell\\MyZip\\command";
static const wchar_t* KEY_DIR =
    L"Software\\Classes\\Directory\\shell\\MyZip\\command";

std::wstring getExePath()
{
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return buf;
}

bool writeCommand(const wchar_t* key, const std::wstring& exePath)
{
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, key, 0, nullptr, 0,
                        KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS)
        return false;

    std::wstring cmd = L"\"" + exePath + L"\" \"%1\"";
    LONG r = RegSetValueExW(hKey, nullptr, 0, REG_SZ,
                            (BYTE*)cmd.c_str(),
                            (DWORD)((cmd.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(hKey);
    return r == ERROR_SUCCESS;
}

std::wstring readCommand(const wchar_t* key)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, key, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return L"";

    wchar_t buf[1024];
    DWORD size = sizeof(buf);
    LONG r = RegQueryValueExW(hKey, nullptr, nullptr, nullptr,
                              (BYTE*)buf, &size);
    RegCloseKey(hKey);

    if (r != ERROR_SUCCESS)
        return L"";

    return buf;
}

void ensureRegistered() {
    std::wstring exe = getExePath();

    auto fix = [&](const wchar_t* key) {
        std::wstring existing = readCommand(key);
        std::wstring expected = L"\"" + exe + L"\" \"%1\"";

        if (existing.empty()) {
            writeCommand(key, exe);
        } else if (!StrStrIW(existing.c_str(), exe.c_str())) {
            writeCommand(key, exe);
        }
    };

    fix(KEY_FILE);
    fix(KEY_DIR);
}

int main(int argc, char** argv) {
    // Register/update context menu entry
    ensureRegistered();

    // Process selected files/folders
    std::cout << "Selected items:\n";
    for (int i = 1; i < argc; ++i) {
        std::cout << "  " << argv[i] << "\n";
        // TODO: ZIP logic here
    }

    return 0;
}

*/
