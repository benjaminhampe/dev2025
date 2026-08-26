#include <de/win32/win32_RegUtil.h>

/*

    static const wchar_t* KEY_FILE =
        L"Software\\Classes\\*\\shell\\MyZip\\command";
    static const wchar_t* KEY_DIR =
        L"Software\\Classes\\Directory\\shell\\MyZip\\command";

    static std::wstring getExePath()
    {
        wchar_t buf[MAX_PATH];
        GetModuleFileNameW(nullptr, buf, MAX_PATH);
        return buf;
    }

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
