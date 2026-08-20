/*

HKEY_CURRENT_USER
 └─ Software
     └─ Classes
         └─ *
             └─ shell
                 └─ 8-zip Compress
                     ├─ (Default) = 8-zip Compress
                     └─ command
                         └─ (Default) = "C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe" "%1"


Computer\HKEY_CURRENT_USER\Software\Classes\*\shell\8-zip Compress\command


[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress]
@="8-zip Compress"
"Icon"="C:\\GitHub\\dev2025_zstd_fltk\\bin\\win64_Release_static\\zstd_fltk.exe"

[HKEY_CURRENT_USER\Software\Classes\Directory\shell\8-zip Compress\command]
@="\"C:\\GitHub\\dev2025_zstd_fltk\\bin\\win64_Release_static\\zstd_fltk.exe\" \"%1\""

🌲 ASCII‑Tree für Ordner‑Kontextmenü (HKCU)
Code

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ Directory
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe"
               └─ command
                  └─ (Default)     REG_SZ   "\"C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe\" \"%1\""

📦 ASCII‑Tree für Datei‑Kontextmenü (HKCU) zum Vergleich
Code

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      └─ *
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               └─ command
                  └─ (Default)     REG_SZ   "\"C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe\" \"%1\""

📌 Beide Trees zusammen (Datei + Ordner)
Code

HKEY_CURRENT_USER
└─ Software
   └─ Classes
      ├─ *
      │  └─ shell
      │     └─ 8-zip Compress
      │        ├─ (Default)        REG_SZ   "8-zip Compress"
      │        └─ command
      │           └─ (Default)     REG_SZ   "\"C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe\" \"%1\""
      └─ Directory
         └─ shell
            └─ 8-zip Compress
               ├─ (Default)        REG_SZ   "8-zip Compress"
               ├─ Icon             REG_SZ   "C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe"
               └─ command
                  └─ (Default)     REG_SZ   "\"C:\GitHub\dev2025_zstd_fltk\bin\win64_Release_static\zstd_fltk.exe\" \"%1\""

Wenn du willst, bau ich dir auch einen ASCII‑Tree für Directory\Background (Rechtsklick im Ordner‑Hintergrund) oder einen Submenü‑Tree wie 7‑Zip.


how do i register my zip program to windows context menu like 7zip

You register your own ZIP tool in the Windows context menu by adding shell extension entries to the registry. 7‑Zip does this through a COM shell extension DLL, but you don’t need a COM extension if you only want simple “Compress with MyZip” menu items — you can register static verbs.

The concise takeaway:
Add registry keys under HKCR\*\shell\YourApp and HKCR\Directory\shell\YourApp pointing to your EXE with the selected file(s) as arguments.  
This gives you right‑click menu entries like 7‑Zip’s basic ones.
🧩 What Windows actually looks for

Windows context menu entries come from three places:

    HKCR\*\shell — items for any file

    HKCR\Directory\shell — items for folders

    HKCR\AllFilesystemObjects\shell — items for both files and folders

    COM shell extensions — dynamic menus (what 7‑Zip uses for its full menu)

For a custom ZIP tool, static verbs are enough unless you need dynamic submenus.

    How to use the Windows Registry Editor
    Windows Registry Guide
    Windows 11 File Explorer’s modern context is getting “New” entry in ...
    How to Customize the Windows 10 Context Menu: Add, Remove Items and ...

🛠️ Minimal static context‑menu registration (recommended)

This adds a simple “Compress to .zip” entry for files and folders.
1. File context menu

Create:
Code

HKEY_CLASSES_ROOT\*\shell\MyZip

Set:

    (Default) = Compress with MyZip

    Icon = C:\Path\To\MyZip.exe

Then create:
Code

HKEY_CLASSES_ROOT\*\shell\MyZip\command

Set:

    (Default) = "C:\Path\To\MyZip.exe" "%1"

2. Folder context menu

Same idea:
Code

HKEY_CLASSES_ROOT\Directory\shell\MyZip

Code

HKEY_CLASSES_ROOT\Directory\shell\MyZip\command

Command:
Code

"C:\Path\To\MyZip.exe" "%1"

This gives you a simple entry like 7‑Zip’s “Add to archive…”.
🧰 If you want a submenu like 7‑Zip (“7‑Zip > Add to archive…”)

You can create a flyout:
Code

HKEY_CLASSES_ROOT\*\shell\MyZip

Add:

    MUIVerb = MyZip

    SubCommands = MyZip.Add;MyZip.ZipHere

Then define each subcommand:
Code

HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\CommandStore\shell\MyZip.Add

Code

HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\CommandStore\shell\MyZip.Add\command

Command:
Code

"C:\Path\To\MyZip.exe" --add "%1"

And similarly for MyZip.ZipHere.

This gives you a submenu without writing a COM extension.
🧩 If you want full dynamic menus like 7‑Zip

Then you must implement a COM shell extension:

    Implement IContextMenu, IShellExtInit

    Register your DLL under
    HKCR\*\shellex\ContextMenuHandlers\YourApp

    Export DllRegisterServer / DllUnregisterServer

This is more work but gives you dynamic items based on selection.

    Remove Edit with Clipchamp from Windows 11 Context Menu
    Effective Shell Interlude: Understanding the Shell
    How to use ShellExView on Windows to disable Shell Extensions
    Create shell context menu for your application

🧪 Testing

After adding registry keys:

    Restart Explorer.exe

    Right‑click files/folders

    Your menu item should appear immediately

🔧 Common pitfalls

    Using HKCU instead of HKCR → menu won’t show for all apps

    Wrong quoting → "MyZip.exe" "%1" must be exact

    Using Icon with a nonexistent path → menu entry appears blank

    Forgetting folder registration → only files get the menu

Next step

Do you want:

    A simple static entry

    A submenu like 7‑Zip

    Or a full COM shell extension

Tell me which one you want and I’ll give you the exact implementation code.

You can jump directly to one of these:

    static context menu

    submenu registration

    COM shell extension

write program that registers entry in explorer context menu, if not already exist.
if already exist still read and check if the program wasnt moved during last call and update registry entry accordingly. 

Explain how i relay a selected number of files and folders in explorer to my context menu entry and how it starts my program and how i get the filenames inside my program to process them

Takeaway:  
You register your ZIP tool by writing a small program that creates or updates registry keys under HKCR\*\shell and HKCR\Directory\shell. Explorer will pass all selected files/folders to your program as command‑line arguments ("%1" "%2" "%3" …). Your program simply reads argv[] and processes them.

Below is a complete, deterministic, implementation‑ready C++ program that:

    Registers your context‑menu entry if missing

    Checks if the EXE path changed

    Updates the registry entry if needed

    Receives multiple selected files/folders from Explorer

    Processes them inside your program

This is exactly the architecture you want for a ZIP tool.
🧩 How Explorer passes selected files/folders to your program

When you register:
Code

"YourZip.exe" "%1"

Explorer expands this to:
Code

YourZip.exe "C:\file1.txt" "C:\file2.png" "C:\folderA"

Every selected item becomes one argument.
You do not need COM, no shell extension, no special API.

Inside your program:
cpp

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string path = argv[i];
        // process file/folder
    }
}

That’s all.
🛠️ Minimal registry structure you will manage
Code

HKCR\*\shell\MyZip
    (Default) = "Compress with MyZip"
    Icon = "C:\Path\To\MyZip.exe"
    command
        (Default) = "\"C:\Path\To\MyZip.exe\" \"%1\""

HKCR\Directory\shell\MyZip
    (same)

🧱 Full C++ program: register + update + process files

This program:

    Detects its own EXE path

    Writes registry entries if missing

    Updates them if the EXE moved

    Prints all selected files/folders (you replace with ZIP logic)

cpp

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

std::wstring getExePath() {
    wchar_t buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return buf;
}

bool writeCommand(const wchar_t* key, const std::wstring& exePath) {
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

std::wstring readCommand(const wchar_t* key) {
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

📌 How this works
✔ Detect EXE location

GetModuleFileNameW gives the current path.
✔ Registry auto‑repair

If the EXE moved, the stored command no longer matches → program rewrites it.
✔ Multiple selections

Explorer calls your program once with all selected items as arguments.
✔ No COM shell extension needed

Static verbs are enough for ZIP tools.
🧭 Optional: submenu like 7‑Zip

If you want a submenu:

    Use CommandStore

    Add multiple commands

    Add SubCommands under your main verb

You can jump directly to that:
submenu registration
🧭 Optional: full dynamic menu (COM)

If you want dynamic entries based on selection:
COM shell extension
🔧 Next step

Do you want me to extend this into:

    A header‑only registry helper

    A full ZIP tool skeleton

    A submenu version

    A COM extension version

Pick one:
registry helper  
zip tool skeleton  
submenu version
imagine i have 2 shortcuts, one for compression one for extraction, how does my program differentiate those actions from ame program

You differentiate the two actions purely by the registry command line.
Explorer does not magically tell your program “this was the Compress shortcut” or “this was the Extract shortcut”.
You decide that yourself by registering different command lines for each menu entry.

This is exactly how 7‑Zip does it.
🎯 Core idea

Each context‑menu entry points to the same EXE, but with different arguments:

    Compress entry:  
    "MyZip.exe" --compress "%1"

    Extract entry:  
    "MyZip.exe" --extract "%1"

Your program checks argv[1] and knows which action to perform.
🧩 How Explorer calls your program

If the user selects 3 files and clicks Compress, Explorer executes:
Code

MyZip.exe --compress "C:\file1.txt" "C:\file2.png" "C:\folderA"

If the user selects 3 files and clicks Extract, Explorer executes:
Code

MyZip.exe --extract "C:\file1.zip" "C:\file2.rar" "C:\folderA"

Your program receives:
Code

argv[0] = MyZip.exe
argv[1] = --compress   OR   --extract
argv[2] = C:\file1.txt
argv[3] = C:\file2.png
argv[4] = C:\folderA
...

That’s the entire mechanism.
🛠️ Registry layout for two menu entries
Compress entry
Code

HKCR\*\shell\MyZipCompress
    MUIVerb = "Compress with MyZip"
    Icon = "C:\Path\MyZip.exe"
    command
        (Default) = "\"C:\Path\MyZip.exe\" --compress \"%1\""

Extract entry
Code

HKCR\*\shell\MyZipExtract
    MUIVerb = "Extract with MyZip"
    Icon = "C:\Path\MyZip.exe"
    command
        (Default) = "\"C:\Path\MyZip.exe\" --extract \"%1\""

Same EXE, different arguments.
🧱 Inside your program: differentiate actions
cpp

int main(int argc, char** argv) {
    if (argc < 2) return 0;

    std::string mode = argv[1];

    std::vector<std::string> items;
    for (int i = 2; i < argc; ++i)
        items.push_back(argv[i]);

    if (mode == "--compress") {
        // compress items
    }
    else if (mode == "--extract") {
        // extract items
    }
}

This is exactly how professional tools do it.
📌 How to support multiple selected files/folders

Explorer expands %1 into all selected items automatically.

You do not need %2, %3, etc.
Explorer handles that for you.

Your program simply reads argv[2] … argv[n].
🧭 How your registration program updates both entries

Your registration/update program should:

    Detect its own EXE path

    Build two command strings:

        "C:\Path\MyZip.exe" --compress "%1"

        "C:\Path\MyZip.exe" --extract "%1"

    Write/update both registry keys

    Compare existing values and update if the EXE moved

This is identical to the previous example, just with two keys.

*/