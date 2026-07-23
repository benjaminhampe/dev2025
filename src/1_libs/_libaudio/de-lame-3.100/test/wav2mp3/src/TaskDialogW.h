#include <windows.h>
#include <commctrl.h>
#include <string>

#pragma comment(lib, "comctl32.lib")

struct EditContext {
    HWND hEdit = nullptr;
    std::wstring result;
};

HRESULT CALLBACK TaskDlgCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData)
{
    EditContext* ctx = reinterpret_cast<EditContext*>(lpRefData);

    switch (msg)
    {
    case TDN_DIALOG_CONSTRUCTED:
    {
        // Create EDIT control inside TaskDialog window
        ctx->hEdit = CreateWindowExW(
            WS_EX_CLIENTEDGE,
            L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
            20, 60, 300, 24,
            hwnd,
            nullptr,
            GetModuleHandleW(nullptr),
            nullptr
        );
        break;
    }

    case TDN_BUTTON_CLICKED:
    {
        // OK button pressed
        if (wParam == IDOK && ctx->hEdit) {
            wchar_t buf[512];
            GetWindowTextW(ctx->hEdit, buf, 512);
            ctx->result = buf;
        }
        break;
    }
    }

    return S_OK;
}

bool TaskDialogInputBox(const std::wstring& title,
                        const std::wstring& instruction,
                        std::wstring& outText)
{
    EditContext ctx;

    TASKDIALOGCONFIG cfg = { 0 };
    cfg.cbSize = sizeof(cfg);
    cfg.hwndParent = nullptr;
    cfg.hInstance = GetModuleHandleW(nullptr);
    cfg.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_SIZE_TO_CONTENT;
    cfg.pszWindowTitle = title.c_str();
    cfg.pszMainInstruction = instruction.c_str();
    cfg.pszContent = L"Bitte Text eingeben:";
    cfg.pfCallback = TaskDlgCallback;
    cfg.lpCallbackData = (LONG_PTR)&ctx;

    TASKDIALOG_BUTTON buttons[] = {
        { IDOK, L"OK" },
        { IDCANCEL, L"Abbrechen" }
    };

    cfg.pButtons = buttons;
    cfg.cButtons = ARRAYSIZE(buttons);
    cfg.nDefaultButton = IDOK;

    int buttonPressed = 0;
    TaskDialogIndirect(&cfg, &buttonPressed, nullptr, nullptr);

    if (buttonPressed == IDOK) {
        outText = ctx.result;
        return true;
    }

    return false;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    std::wstring text;

    if (TaskDialogInputBox(L"Input", L"Gib deinen Text ein:", text)) {
        MessageBoxW(nullptr, text.c_str(), L"Result", MB_OK);
    }

    return 0;
}
