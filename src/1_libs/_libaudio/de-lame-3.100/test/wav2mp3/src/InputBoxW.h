// InputBoxW wrapper — minimal, efficient, no bullshit.
// Returns true if user entered text, false if cancelled.

#include <windows.h>
#include <string>

typedef int (__stdcall *InputBoxW_t)(
    wchar_t* prompt,
    wchar_t* title,
    wchar_t* defaultText,
    wchar_t* buffer,
    int bufferSize
);

bool InputBoxWWrapper(const std::wstring& prompt,
                      const std::wstring& title,
                      const std::wstring& defaultText,
                      std::wstring& outText,
                      int maxLen = 1024)
{
    HMODULE hMod = LoadLibraryW(L"msvbvm60.dll");
    if (!hMod)
        return false;

    auto fn = reinterpret_cast<InputBoxW_t>(
        GetProcAddress(hMod, "InputBoxW")
    );

    if (!fn) {
        FreeLibrary(hMod);
        return false;
    }

    std::wstring buffer;
    buffer.resize(maxLen);

    int ret = fn(
        const_cast<wchar_t*>(prompt.c_str()),
        const_cast<wchar_t*>(title.c_str()),
        const_cast<wchar_t*>(defaultText.c_str()),
        &buffer[0],
        maxLen
    );

    FreeLibrary(hMod);

    if (ret == 0)
        return false; // user cancelled

    // Trim null terminator
    buffer.resize(wcslen(buffer.c_str()));
    outText = buffer;
    return true;
}

#include <iostream>

int main()
{
    std::wstring result;

    if (InputBoxWWrapper(L"Gib deinen Text ein:",
                         L"Input",
                         L"Default",
                         result))
    {
        std::wcout << L"User entered: " << result << L"\n";
    }
    else
    {
        std::wcout << L"User cancelled.\n";
    }

    return 0;
}
