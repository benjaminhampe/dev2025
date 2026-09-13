#include "win32_Get_Explorer_Selection.h"

#include <windows.h>
#include <exdisp.h>
#include <shlobj.h>
#include <shldisp.h>
#include <vector>
#include <string>

AutoCoInitialize::AutoCoInitialize()
{
    // COM initialisieren
    CoInitialize(NULL);
}

AutoCoInitialize::~AutoCoInitialize()
{
    CoUninitialize();
}

std::vector<std::wstring> win32_Get_Explorer_Selection()
{
    std::vector<std::wstring> result;

    // COM initialisieren
    //CoInitialize(NULL);

    IShellWindows* shellWindows = nullptr;

    HRESULT hr = CoCreateInstance(
        CLSID_ShellWindows,
        NULL,
        CLSCTX_ALL,
        IID_PPV_ARGS(&shellWindows)
    );

    if (FAILED(hr) || !shellWindows)
        return result;

    long count = 0;
    shellWindows->get_Count(&count);

    for (long i = 0; i < count; i++)
    {
        VARIANT v;
        VariantInit(&v);
        v.vt = VT_I4;
        v.lVal = i;

        IDispatch* disp = nullptr;
        if (FAILED(shellWindows->Item(v, &disp)) || !disp)
            continue;

        IWebBrowserApp* browser = nullptr;
        hr = disp->QueryInterface(IID_IWebBrowserApp, (void**)&browser);
        disp->Release();

        if (FAILED(hr) || !browser)
            continue;

        // Dokument holen → ShellFolderViewDual
        IDispatch* docDisp = nullptr;
        hr = browser->get_Document(&docDisp);
        browser->Release();

        if (FAILED(hr) || !docDisp)
            continue;

        IShellFolderViewDual* view = nullptr;
        hr = docDisp->QueryInterface(IID_IShellFolderViewDual, (void**)&view);
        docDisp->Release();

        if (FAILED(hr) || !view)
            continue;

        // Ausgewählte Items holen
        FolderItems* items = nullptr;
        hr = view->SelectedItems(&items);
        view->Release();

        if (FAILED(hr) || !items)
            continue;

        long itemCount = 0;
        items->get_Count(&itemCount);

        for (long j = 0; j < itemCount; j++)
        {
            VARIANT idx;
            VariantInit(&idx);
            idx.vt = VT_I4;
            idx.lVal = j;

            FolderItem* item = nullptr;
            if (SUCCEEDED(items->Item(idx, &item)) && item)
            {
                BSTR bstrPath;
                if (SUCCEEDED(item->get_Path(&bstrPath)))
                {
                    result.emplace_back(bstrPath);
                    SysFreeString(bstrPath);
                }
                item->Release();
            }
        }

        items->Release();
    }

    shellWindows->Release();
    // CoUninitialize();

    return result;
}
