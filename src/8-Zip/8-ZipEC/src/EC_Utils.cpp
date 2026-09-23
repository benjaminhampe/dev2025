#include "EC_Utils.h"
#include <filesystem>
#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <shellapi.h>

void LogEvent(const std::wstring& msg)
{
    HANDLE h = RegisterEventSourceW(NULL, L"8-ZipEC");
    if (!h) return;

    const wchar_t* strings[1] = { msg.c_str() };

    ReportEventW(
        h,                      // HANDLE
        EVENTLOG_INFORMATION_TYPE,  // wType
        0,                     // wCategory
        0,                     // dwEventID
        NULL,                  // lpUserSid
        1,                     // wNumStrings
        0,                     // dwDataSize
        strings,               // lpStrings
        NULL                   // lpRawData
    );

    DeregisterEventSource(h);
}

void LogEvents(const std::vector<std::wstring>& msg)
{
    HANDLE h = RegisterEventSourceW(NULL, L"8-ZipEC");
    if (!h) return;

    std::vector<const wchar_t*> lines( msg.size() );
    for (size_t i = 0; i < msg.size(); ++i)
    {
        lines[i] = msg[i].data();
    }

    ReportEventW(
        h,                      // HANDLE
        EVENTLOG_INFORMATION_TYPE,  // wType
        0,                     // wCategory
        0,                     // dwEventID
        NULL,                  // lpUserSid
        (WORD)lines.size(),    // wNumStrings
        0,                     // dwDataSize
        lines.data(),          // lpStrings
        NULL                   // lpRawData
    );

    DeregisterEventSource(h);
}


std::wstring EightZip_Registry_readExePath()
{
    HKEY hKey;
    DWORD r = RegOpenKeyExW(HKEY_CURRENT_USER,
        L"Software\\8-Zip", 0, KEY_READ, &hKey);
    if (r != ERROR_SUCCESS)
    {
        return {};
    }

    DWORD type = 0;
    DWORD size = 0;

    // First call: get required buffer size
    r = RegQueryValueExW(hKey, L"Path", nullptr, &type, nullptr, &size);
    if (r != ERROR_SUCCESS || type != REG_SZ)
    {
        RegCloseKey(hKey);
        return {};
    }

    // Allocate buffer (size is in bytes)
    std::wstring out;
    out.resize(size / sizeof(wchar_t));

    // Second call: read actual data
    r = RegQueryValueExW(
        hKey,
        L"Path",
        nullptr,
        nullptr,
        reinterpret_cast<LPBYTE>(&out[0]),
        &size
    );

    RegCloseKey(hKey);

    if (r != ERROR_SUCCESS)
        return {};

    // Remove trailing null if present
    while (!out.empty() && out.back() == L'\0')
    {
        out.pop_back();
    }

    return out;
}

void EightZip_runCompressor(const StringListW& files)
{
    LogEvent(L"8-ZipEC :: [Ok] runCompressor.");

    std::wstring exePath = EightZip_Registry_readExePath();
    if (exePath.empty())
    {
        LogEvent(L"8-ZipEC :: [Error] No 8-Zip exePath in registry.");
        return;
    }

    std::wstring cmdLine = L"\"" + exePath + L"\"";
    std::wstring args = L" --gui --compress";

    // We don't send selection of files and directories (anymore).
    // 8z.exe queries them with win32_Get_Explorer_Selection().

    // for (auto& s : files)
    // {
    //     args += L" \"" + s + L"\"";
    // }

    ShellExecuteW(
        NULL,
        NULL, // L"open",
        cmdLine.c_str(),
        args.c_str(),
        NULL,
        SW_SHOW);
}

void EightZip_runExtractor(const StringListW& files)
{
    LogEvent(L"8-ZipEC :: [Ok] runExtractor.");

    std::wstring exePath = EightZip_Registry_readExePath();

    if (exePath.empty())
    {
        LogEvent(L"8-ZipEC :: [Error] No 8-Zip exePath in registry.");
        return;
    }

    std::wstring cmdLine = L"\"" + exePath + L"\"";
    std::wstring args = L" --gui --extract";

    // We don't send selection of files and directories (anymore).
    // 8z.exe queries them with win32_Get_Explorer_Selection().

    // for (auto& s : files)
    // {
    //     args += L" \"" + s + L"\"";
    // }

    ShellExecuteW(
        NULL,
        NULL, // L"open",
        cmdLine.c_str(),
        args.c_str(),
        NULL,
        SW_SHOW);
}

bool IsArchive(const std::wstring& ext)
{
    return
        ext == L".tar" ||
        ext == L".zst" || ext == L".zstd" ||
        ext == L".zip" || ext == L".zipx" || ext == L".jar" || ext == L".xpi" ||
        ext == L".7z" ||
        ext == L".bz2" || ext == L".bzip2" ||ext == L".tbz" ||ext == L".tbz2" ||
        ext == L".gz" || ext == L".gzip" || ext == L".tgz" ||
        ext == L".xz" || ext == L".txz" ||
        ext == L".rar";
}


// 🧩 Step 2 — Classify selection
StringListW GetFilteredArchives(const StringListW& iList)
{
    StringListW oList;
    oList.reserve( iList.size() );

    for (const auto& iName : iList)
    {
        const auto p = std::filesystem::path(iName);

        const std::filesystem::file_status fs = std::filesystem::status(p);

        if (!std::filesystem::exists( fs ))
        {
            continue;
        }

        if (!std::filesystem::is_regular_file( fs ))
        {
            continue;
        }

        auto ext = p.extension().wstring();

        std::transform(ext.begin(),
                       ext.end(),
                       ext.begin(),
                       ::towlower);

        if (!IsArchive( ext ))
        {
            continue;
        }

        oList.emplace_back(iName);
    }

    return oList;
}

/*
// 🧩 Step 2 — Classify selection
StringListW
EightZip_classifySelection(
    const StringListW& inputList,
    uint32_t& numDirs,
    uint32_t& numFiles,
    uint32_t& numArchives )
{
    size_t n = inputList.size();

    StringListW outputList;
    outputList.reserve( n );

    std::wostringstream o; o <<
    "8-ZipSE [classifySelection]\n"
    "inputList = " << n << "\n";

    for (size_t i = 0; i < n; ++i)
    {
        const auto& p = inputList[i];

        const auto fs_path = std::filesystem::path(p);

        const std::filesystem::file_status fs = std::filesystem::status( fs_path );

        if (!std::filesystem::exists( fs ))
        {
            o << "[Error] Not exist, " << p << "\n";
            continue;
        }

        const bool bDirectory = std::filesystem::is_directory( fs );
        const bool bRegularFile = std::filesystem::is_regular_file( fs );

        if (!bDirectory && !bRegularFile)
        {
            o << "[Error] Skip " << p << "\n";
            continue;
        }

        outputList.emplace_back(p);

        if (bDirectory)
        {
            o << "Directory[" << numDirs << "] " << p << "\n";
            numDirs++;
        }
        else
        {
            std::wstring ext = fs_path.extension().wstring();

            std::transform(ext.begin(),
                           ext.end(),
                           ext.begin(),
                           ::towlower);

            o << "File[" << numFiles << "] [" << ext << "] " << p << "\n";
            numFiles++;

            if (ext == L".tar")
            {
                numArchives++;
            }
            else if (ext == L".zst" || ext == L".zstd")
            {
                numArchives++;
            }
            else if (ext == L".zip" || ext == L".zipx" || ext == L".jar" || ext == L".xpi")
            {
                numArchives++;
            }
            else if (ext == L".7z")
            {
                numArchives++;
            }
            else if (ext == L".bz2" || ext == L".bzip2" || ext == L".tbz" || ext == L".tbz2")
            {
                numArchives++;
            }
            else if (ext == L".gz" || ext == L".gzip" || ext == L".tgz")
            {
                numArchives++;
            }
            else if (ext == L".xz" || ext == L".txz")
            {
                numArchives++;
            }
        }
    }

    o <<
    "numDirs = " << numDirs << "\n"
    "numFiles = " << numFiles << "\n"
    "numArchives = " << numArchives << "\n"
    "\n";

    LogEvents(de::StringUtil::split(o.str(),L'\n',true));

    return outputList;
}

*/
