#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commctrl.h> // For toolbar
#include <sstream>
#include "../res/resource.h"
// Icon resources
#define IDI_ICON1 101
#define IDI_ICON2 102

// Menu items
#define IDM_FILE_NEW 40001
#define IDM_FILE_OPEN 40002
#define IDM_FILE_EXIT 40003

// Toolbar buttons
#define IDT_NEW 50001
#define IDT_OPEN 50002
#define IDT_SAVE 50003
#define IDT_BUTTON1 50101
#define IDT_BUTTON2 50102
#define IDT_BUTTON_TOGGLE_FPS 50103

// Canvas actions
#define IDC_CANVAS 60001
#define IDC_SCROLLBAR_HORZ 60002
#define IDC_SCROLLBAR_VERT 60003
#define IDC_ZOOM 60004
#define IDT_TIMER_FPS 1

// Function declarations
void UpdateLayout();
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CanvasProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void RegisterCustomControl();
void AddMenus(HWND hwnd);
void AddToolbar(HWND hwnd);
void ToggleFPSTimer(HWND hwnd);

// Global variables
HINSTANCE g_hInstance;
HWND g_hWindow;
//HWND g_canvas;
HWND g_scrollbarH;
HWND g_scrollbarV;
int scrollPosX = 0;
int scrollPosY = 0;
int g_screenWidth = 0;
int g_screenHeight = 0;
BOOL fpsEnabled = FALSE;

HMODULE GetThisModuleHandle()
{
    //Returns module handle where this function is running in: EXE or DLL
    HMODULE hModule = NULL;
    ::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)GetThisModuleHandle, &hModule);

    return hModule;
}

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
int main(int argc, char* argv[])
{

    // BOOL ShowWindow(
    //     [in] HWND hWnd,
    //     [in] int  nCmdShow
    //     );
    //         Wert	Bedeutung
    // SW_HIDE
    // 0	Blendet das Fenster aus und aktiviert ein anderes Fenster.
    // SW_SHOWNORMAL
    // SW_NORMAL
    // 1	Aktiviert und zeigt ein Fenster an. Wenn das Fenster minimiert,
    //      maximiert oder angeordnet ist, wird es vom System auf seine ursprüngliche
    //      Größe und Position wiederhergestellt. Eine Anwendung sollte dieses Flag
    //      angeben, wenn das Fenster zum ersten Mal angezeigt wird.
    // SW_SHOWMINIMIZED
    // 2	Aktiviert das Fenster und zeigt es als minimiertes Fenster an.
    // SW_SHOWMAXIMIZED
    // SW_MAXIMIZE
    // 3	Aktiviert das Fenster und zeigt es als maximiertes Fenster an.
    // SW_SHOWNOACTIVATE
    // 4	Zeigt ein Fenster in seiner neuesten Größe und Position an.
    //      Dieser Wert ähnelt SW_SHOWNORMAL, mit der Ausnahme,
    //      dass das Fenster nicht aktiviert ist.
    // SW_SHOW
    // 5	Aktiviert das Fenster und zeigt es in seiner aktuellen Größe und Position an.
    // SW_MINIMIZE
    // 6	Minimiert das angegebene Fenster und aktiviert das nächste Fenster der
    //      obersten Ebene in der Reihenfolge Z.
    // SW_SHOWMINNOACTIVE
    // 7	Zeigt das Fenster als minimiertes Fenster an. Dieser Wert ähnelt
    //      SW_SHOWMINIMIZED, außer dass das Fenster nicht aktiviert ist.
    // SW_SHOWNA
    // 8	Zeigt das Fenster in seiner aktuellen Größe und Position an.
    //      Dieser Wert ähnelt SW_SHOW, mit der Ausnahme, dass das Fenster nicht
    //      aktiviert ist.
    // SW_RESTORE
    // 9	Aktiviert das Fenster und zeigt es an. Wenn das Fenster minimiert,
    //      maximiert oder angeordnet ist, wird es vom System auf seine ursprüngliche
    //      Größe und Position wiederhergestellt. Eine Anwendung sollte dieses Flag
    //      beim Wiederherstellen eines minimierten Fensters angeben.
    // SW_SHOWDEFAULT
    // 10	Legt den Showstatus basierend auf dem SW_ Wert fest, der in der
    //      STARTUPINFO-Struktur angegeben ist, die von dem Programm, dass die
    //      Anwendung gestartet hat, an die CreateProcess-Funktion übergeben wird.
    // SW_FORCEMINIMIZE
    // 11	Minimiert ein Fenster, auch wenn der Thread, der das Fenster besitzt,
    //      nicht reagiert. Dieses Flag sollte nur verwendet werden,
    //      wenn Fenster aus einem anderen Thread minimiert werden.
    int nCmdShow = SW_NORMAL;
    g_hInstance = GetModuleHandle(0);

    // Register window class
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"MainWindowClass";
    wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_ICON1));
    RegisterClass(&wc);

    // Register custom control for canvas
    RegisterCustomControl();

    // Create the window
    HWND hwnd = CreateWindowEx(0, L"MainWindowClass", L"Win32 API Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, nullptr, nullptr, g_hInstance, nullptr);

    g_hWindow = hwnd;
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

void UpdateLayout()
{
    int w = g_screenWidth;
    int h = g_screenHeight;

    RECT rc_win;
    GetClientRect(g_hWindow, &rc_win);

    int rc_w = rc_win.right - rc_win.left;
    int rc_h = rc_win.bottom - rc_win.top;
    int sb_w = 40;
    int sb_h = 40;
    //MoveWindow(g_canvas, rc_win.left, rc_win.top, rc_w - sb_w, rc_h - sb_h, TRUE);
    MoveWindow(g_scrollbarV, rc_win.right - sb_w, rc_win.top, sb_w, rc_h - sb_h, TRUE);
    MoveWindow(g_scrollbarH, rc_win.left, rc_win.bottom - sb_h, rc_w - sb_w, sb_h, TRUE);

    //InvalidateRect(g_hWindow, nullptr, TRUE);
}
// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {

            InitCommonControls();

            //MessageBox( hWndParent, L"No Tool Images", L"BOB", MB_OK );

            // AddMenus(hwnd);
            HMENU hMenu = CreateMenu();
            HMENU hSubMenu = CreateMenu();
            AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, L"File");
            AppendMenu(hSubMenu, MF_STRING, IDM_FILE_NEW, L"New");
            AppendMenu(hSubMenu, MF_STRING, IDM_FILE_OPEN, L"Open");
            AppendMenu(hSubMenu, MF_STRING, IDM_FILE_EXIT, L"Exit");
            SetMenu(hwnd, hMenu);

            // AddToolbar(hwnd);
            HWND hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, nullptr,
                                           WS_CHILD | WS_VISIBLE | TBSTYLE_WRAPABLE,
                                           0, 0, 0, 0, hwnd, nullptr, g_hInstance, nullptr);

/*
            TBADDBITMAP tbab = { 0 };
            tbab.hInst = HINST_COMMCTRL;
            tbab.nID = IDB_STD_SMALL_COLOR;
            SendMessage(hToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);

            // Add buttons to toolbar
            TBBUTTON tbb[3] = { 0 };
            tbb[0].iBitmap = MAKELONG(STD_FILENEW, 0);
            tbb[0].idCommand = IDT_BUTTON1;
            tbb[0].fsState = TBSTATE_ENABLED;
            tbb[0].fsStyle = TBSTYLE_BUTTON;

            tbb[1].iBitmap = MAKELONG(STD_FILEOPEN, 0);
            tbb[1].idCommand = IDT_BUTTON2;
            tbb[1].fsState = TBSTATE_ENABLED;
            tbb[1].fsStyle = TBSTYLE_BUTTON;

            tbb[2].iBitmap = MAKELONG(STD_REDOW, 0);
            tbb[2].idCommand = IDT_BUTTON_TOGGLE_FPS;
            tbb[2].fsState = TBSTATE_ENABLED;
            tbb[2].fsStyle = TBSTYLE_BUTTON;
            SendMessage(hToolbar, TB_ADDBUTTONS, 3, (LPARAM)&tbb);
*/
            // ======================================================================

            // ======================================================================


            // Declare and initialize local constants.
            const int ImageListID    = 0;
            const int bitmapSize     = 16;
            const DWORD buttonStyles = BTNS_AUTOSIZE;

            TBBUTTON tbButtons[4] =
                {
                    { MAKELONG(aaaa, ImageListID), IDT_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New" },
                    { MAKELONG(aaaa, ImageListID), IDT_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
                    { MAKELONG(aaaa, ImageListID), IDT_SAVE, 0,               buttonStyles, {0}, 0, (INT_PTR)L"Save"},
                    { MAKELONG(aaaa, ImageListID), IDT_SAVE, 0,               buttonStyles, {0}, 0, (INT_PTR)L"SaveAs"}
                };

            auto g_hImageList = ImageList_Create(32, 32,   // Dimensions of individual bitmaps.
                                            ILC_COLOR32,   // | ILC_MASK, Ensures transparent background.
                                            4,0);

            //hiconItem = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ITEM));

            auto hIcon1 = LoadIcon(g_hInstance, MAKEINTRESOURCE(aaaa));
            auto hIcon2 = LoadIcon(g_hInstance, MAKEINTRESOURCE(aaaa));
            auto hIcon3 = LoadIcon(g_hInstance, MAKEINTRESOURCE(aaaa));
            auto hIcon4 = LoadIcon(g_hInstance, MAKEINTRESOURCE(aaaa));
            //auto hIcon = LoadIconA(NULL, "folder.ico");
            //if (hIcon==NULL) return FALSE;
            ImageList_AddIcon(g_hImageList, hIcon1);
            ImageList_AddIcon(g_hImageList, hIcon2);
            ImageList_AddIcon(g_hImageList, hIcon3);
            ImageList_AddIcon(g_hImageList, hIcon4);

            // Set the image list.
            SendMessage(hToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)g_hImageList);

            // Load the button images.
            SendMessage(hToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);

            // Add buttons.
            SendMessage(hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessage(hToolbar, TB_ADDBUTTONS,       (WPARAM)4,       (LPARAM)&tbButtons);

            // Resize the toolbar, and then show it.
            SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
            ShowWindow(hToolbar,  TRUE);

            /*
            // Create the image list.
            g_hImageList = ImageList_Create(bitmapSize, bitmapSize,   // Dimensions of individual bitmaps.
                                            ILC_COLOR16 | ILC_MASK,   // Ensures transparent background.
                                            numButtons, 0);

            // Set the image list.
            SendMessage(hWndToolbar, TB_SETIMAGELIST,
                        (WPARAM)ImageListID,
                        (LPARAM)g_hImageList);

            // Load the button images.
            SendMessage(hWndToolbar, TB_LOADIMAGES,
                        (WPARAM)IDB_STD_SMALL_COLOR,
                        (LPARAM)HINST_COMMCTRL);

            // Initialize button info.
            // IDM_NEW, IDM_OPEN, and IDM_SAVE are application-defined command constants.

            TBBUTTON tbButtons[numButtons] =
                {
                    { MAKELONG(STD_FILENEW,  ImageListID), IDM_NEW,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"New" },
                    { MAKELONG(STD_FILEOPEN, ImageListID), IDM_OPEN, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)L"Open"},
                    { MAKELONG(STD_FILESAVE, ImageListID), IDM_SAVE, 0,               buttonStyles, {0}, 0, (INT_PTR)L"Save"}
                };

            // Add buttons.
            SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            SendMessage(hWndToolbar, TB_ADDBUTTONS,       (WPARAM)numButtons,       (LPARAM)&tbButtons);

            // Resize the toolbar, and then show it.
            SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
            ShowWindow(hWndToolbar,  TRUE);

*/

            // Create canvas custom control
            //g_canvas = CreateWindow(L"CanvasControl", nullptr, WS_CHILD | WS_VISIBLE,
            //    0, 40, 760, 560, hwnd, (HMENU)IDC_CANVAS, g_hInstance, nullptr);

            // Create horizontal scrollbar
            g_scrollbarH = CreateWindowEx(0, L"SCROLLBAR", nullptr, WS_CHILD | WS_VISIBLE | SBS_HORZ,
                0, 600, 760, 20, hwnd, (HMENU)IDC_SCROLLBAR_HORZ, g_hInstance, nullptr);

            // Create vertical scrollbar
            g_scrollbarV = CreateWindowEx(0, L"SCROLLBAR", nullptr, WS_CHILD | WS_VISIBLE | SBS_VERT,
                760, 40, 20, 560, hwnd, (HMENU)IDC_SCROLLBAR_VERT, g_hInstance, nullptr);

            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
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
                case IDT_BUTTON_TOGGLE_FPS:
                    // Toggle 60 FPS timer
                    ToggleFPSTimer(hwnd);
                    break;
            }
            break;
        case WM_HSCROLL:
            if (lParam != 0)
            {
                int position = GetScrollPos((HWND)lParam, SB_CTL);
                switch (LOWORD(wParam)) {
                    case SB_THUMBTRACK:
                    case SB_THUMBPOSITION:
                        position = HIWORD(wParam);
                        break;
                    case SB_LINELEFT:
                        position -= 1;
                        break;
                    case SB_LINERIGHT:
                        position += 1;
                        break;
                    case SB_PAGELEFT:
                        position -= 10;
                        break;
                    case SB_PAGERIGHT:
                        position += 10;
                        break;
                }
                SetScrollPos((HWND)lParam, SB_CTL, position, TRUE);
                scrollPosX = position;
                // Redraw canvas with new scroll position
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        case WM_VSCROLL:
            if (lParam != 0) {
                int position = GetScrollPos((HWND)lParam, SB_CTL);
                switch (LOWORD(wParam)) {
                    case SB_THUMBTRACK:
                    case SB_THUMBPOSITION:
                        position = HIWORD(wParam);
                        break;
                    case SB_LINEUP:
                        position -= 1;
                        break;
                    case SB_LINEDOWN:
                        position += 1;
                        break;
                    case SB_PAGEUP:
                        position -= 10;
                        break;
                    case SB_PAGEDOWN:
                        position += 10;
                        break;
                }
                SetScrollPos((HWND)lParam, SB_CTL, position, TRUE);
                scrollPosY = position;
                // Redraw canvas with new scroll position
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        case WM_TIMER:
            if (wParam == IDT_TIMER_FPS) {
                // Redraw canvas at 60 FPS
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            int w = g_screenWidth;
            int h = g_screenHeight;

            //HBRUSH MyBrush = CreateSolidBrush( RGB( 0, 150, 255 ) );
            HPEN hPen0 = CreatePen(PS_SOLID, 1, RGB(0, 150, 255));
            HBRUSH hBrush0 = CreateSolidBrush(RGB(0, 150, 255));
            SelectObject(hdc, hPen0);
            SelectObject(hdc, hBrush0);
            Rectangle(hdc, 0, 0, w, h);
            DeleteObject(hPen0);
            DeleteObject(hBrush0);

            HPEN hPen2 = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
            SelectObject(hdc, hPen2);

            HBRUSH hBrush1 = CreateSolidBrush(RGB(121, 90, 0));
            SelectObject(hdc, hBrush1);
            Rectangle(hdc, 10, 10, 300, 300);

            DeleteObject(hPen2);
            DeleteObject(hBrush1);

            /*
            int w = 256;
            int h = 256;

            std::vector< uint8_t > pixels( w * h * size_t( 4 ) );
            uint8_t* dst = pixels.data();

            int x = 0;
            int y = 0;
            int cw = 32;
            int ch = 32;
            for ( int j = 0; j < 2*ch; ++j )
            {
            for ( int i = 0; i < 2*cw; ++i )
            {
            *dst++ = 200; *dst++ = 200; *dst++ = 200; *dst++ = 255;
            }
            }

            for ( int j = 0; j < ch; ++j )
            {
            for ( int i = 0; i < cw; ++i )
            {
            *dst++ = 100; *dst++ = 100; *dst++ = 100; *dst++ = 255;
            }
            }

            for ( int j = c; j < ch; ++j )
            {
            for ( int i = 0; i < cw; ++i )
            {
            *dst++ = 100; *dst++ = 100; *dst++ = 100; *dst++ = 255;
            }
            }

            HBITMAP hBitmap = CreateBitmap( w, h, 4, 8, (LPBYTE)tmp.data() );

            HDC mem = CreateCompatibleDC( hdc );
            HBITMAP old = (HBITMAP)SelectObject( mem, hBitmap );

            BITMAP bm;
            GetObject( hBitmap, sizeof( bm ), &bm );
            BitBlt( dc, 0, 0, bm.bmWidth, bm.bmHeight, mem, 0, 0, SRCCOPY );
            SelectObject( mem, old );
            DeleteDC( mem );
            */
            SetTextColor(hdc, RGB( 255,    0, 0));  // rot
            SetBkColor(hdc, RGB( 255, 255,  0));  // gelb
            TextOutA(hdc, 20, 20, "Ich bin ein Fenster.", 20);

            //      int SetBkMode
            //      (
            //          HDC hdc,          // handle of device context
            //          int iBkMode      // flag specifying background mode
            //      );

            //      durch Wahl des Hintergrund-Modus TRANSPARENT die Hintergrundfarbe ignorieren.
            //      Zurückschalten können Sie mit OPAQUE.

            EndPaint (hwnd, &ps);
            return 0;
        }
        case WM_ERASEBKGND:
            return 0;
        case WM_SIZE: {

            int w = int( LOWORD( lParam ) );
            int h = int( HIWORD( lParam ) );

            g_screenWidth = w;
            g_screenHeight = h;
            int dw = GetSystemMetrics( SM_CXSCREEN );
            int dh = GetSystemMetrics( SM_CYSCREEN );

            std::ostringstream o;
            o << "Desktop("<<dw<<","<<dh<<"), Screen(" << w<<","<< h<<")";
            SetWindowTextA( hwnd, o.str().c_str() );

            UpdateLayout();
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
    wc.lpfnWndProc = CanvasProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"CanvasControl";
    RegisterClass(&wc);
}

// Custom canvas control procedure function
LRESULT CALLBACK CanvasProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Custom drawing code here
            TextOut(hdc, -scrollPosX, -scrollPosY, L"Demo Text", 9);

            EndPaint(hwnd, &ps);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void ToggleFPSTimer(HWND hwnd)
{

}
