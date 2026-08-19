#include <windows.h>
#include <commctrl.h> // For toolbar

// Icon resources
#define IDI_ICON1 101
#define IDI_ICON2 102

// Menu items
#define IDM_FILE_NEW 40001
#define IDM_FILE_OPEN 40002
#define IDM_FILE_EXIT 40003

// Toolbar buttons
#define IDT_BUTTON1 50001
#define IDT_BUTTON2 50002

// Canvas actions
#define IDC_CANVAS 60001
#define IDC_SCROLLBAR 60002
#define IDC_ZOOM 60003

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void RegisterCustomControl();
void AddMenus(HWND hwnd);
void AddToolbar(HWND hwnd);

// Global variables
HINSTANCE hInst;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
    hInst = hInstance;
    
    // Register window class
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MainWindowClass";
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClass(&wc);

    // Register custom control for canvas
    RegisterCustomControl();

    // Create the window
    HWND hwnd = CreateWindowEx(0, L"MainWindowClass", L"Win32 API Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, hInstance, nullptr);

    // Display the window
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop
    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Add menu
            AddMenus(hwnd);

            // Add toolbar
            AddToolbar(hwnd);

            // Create canvas custom control
            CreateWindow(L"CanvasControl", nullptr, WS_CHILD | WS_VISIBLE,
                0, 40, 800, 560, hwnd, (HMENU)IDC_CANVAS, hInst, nullptr);

            // Create horizontal scrollbar
            CreateWindowEx(0, L"SCROLLBAR", nullptr, WS_CHILD | WS_VISIBLE | SBS_HORZ,
                0, 600, 800, 20, hwnd, (HMENU)IDC_SCROLLBAR, hInst, nullptr);

            // Create vertical scrollbar
            CreateWindowEx(0, L"SCROLLBAR", nullptr, WS_CHILD | WS_VISIBLE | SBS_VERT,
                780, 40, 20, 560, hwnd, (HMENU)IDC_SCROLLBAR, hInst, nullptr);
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_FILE_NEW:
                    // Handle New action
                    break;
                case IDM_FILE_OPEN:
                    // Handle Open action
                    break;
                case IDM_FILE_EXIT:
                    PostQuitMessage(0);
                    break;
                case IDT_BUTTON1:
                    // Handle toolbar button 1
                    break;
                case IDT_BUTTON2:
                    // Handle toolbar button 2
                    break;
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Handle canvas drawing here

            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Function to register the custom control for the canvas
void RegisterCustomControl() {
    WNDCLASS wc = { };
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"CanvasControl";
    RegisterClass(&wc);
}

// Function to add menus to the window
void AddMenus(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, L"File");
    AppendMenu(hSubMenu, MF_STRING, IDM_FILE_NEW, L"New");
    AppendMenu(hSubMenu, MF_STRING, IDM_FILE_OPEN, L"Open");
    AppendMenu(hSubMenu, MF_STRING, IDM_FILE_EXIT, L"Exit");
    SetMenu(hwnd, hMenu);
}

// Function to add a toolbar to the window
void AddToolbar(HWND hwnd) {
    HWND hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr, WS_CHILD | WS_VISIBLE | TBSTYLE_WRAPABLE,
        0, 0, 0, 0, hwnd, nullptr, hInst, nullptr);
    TBADDBITMAP tbab = { 0 };
    tbab.hInst = HINST_COMMCTRL;
    tbab.nID = IDB_STD_SMALL_COLOR;
    SendMessage(hToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

    // Add buttons to toolbar
    TBBUTTON tbb[2] = { 0 };
    tbb[0].iBitmap = MAKELONG(STD_FILENEW, 0);
    tbb[0].idCommand = IDT_BUTTON1;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON;
    tbb[1].iBitmap = MAKELONG(STD_FILEOPEN, 0);
    tbb[1].idCommand = IDT_BUTTON2;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    SendMessage(hToolbar, TB_ADDBUTTONS, 2, (LPARAM)&tbb);
}
