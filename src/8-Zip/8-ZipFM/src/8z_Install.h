#pragma once
#include <string>

bool EightZip_isAdmin();

bool EightZip_InstallExePath();

bool EightZip_UninstallExePath();


bool EightZip_isInstalled();

bool EightZip_Install(); // 8zipFM.exe --install

bool EightZip_Uninstall(); // 8zipFM.exe --uninstall/ --deinstall


bool EightZip_execute(
        std::wstring cmd,
        std::wstring params = L"",
        bool bFireAndForget = false,
        bool bAdmin = false,
        std::wstring dir = L"");

// Very useful for developing the ShellExtension.dll.
// Explorer needs to be restarted to understand changes, aka newly compiled SE.dll
bool EightZip_restartExplorer();
