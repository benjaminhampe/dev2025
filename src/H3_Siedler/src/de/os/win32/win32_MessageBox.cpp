#include <de/os/win32/win32_MessageBox.h>

// #define UNICODE
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
#define WIN32_NOMINMAX
#endif

#include <windows.h>
#include <string>

namespace de {
namespace {

} // end namespace.
} // end namespace de.

void dbMessageBox( const de::MessageBoxParamsW& params )
{
    HWND hwndParent = (HWND)params.parent;

    MessageBoxW(hwndParent,
        params.title.c_str(),
        params.text.c_str(),
        MB_OK);
}


void dbMessageBox( const std::wstring& title, const std::wstring& text)
{
    de::MessageBoxParamsW params;
    params.title = title;
    params.text = text;
    dbMessageBox( params );
}

bool dbQuestionBox(const std::wstring& title, const std::wstring& text)
{
    return IDOK == MessageBoxW( nullptr, text.c_str(), title.c_str(), MB_OKCANCEL | MB_ICONINFORMATION );
}
