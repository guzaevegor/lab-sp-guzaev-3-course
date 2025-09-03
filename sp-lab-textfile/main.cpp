#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define ID_FILE_OPEN    1001
#define ID_FILE_SAVE    1002  
#define ID_FILE_EXIT    1003
#define ID_EDIT_CUT     1004
#define ID_EDIT_COPY    1005
#define ID_EDIT_PASTE   1006
#define ID_HELP_ABOUT   1007
#define IDM_MYMENURESOURCE   3

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("WinApiTextEditor");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, Windows desktop!");

    switch (message)
    {

    case WM_CREATE:
        MessageBox(NULL,
            _T("Call a WM_CREATE!"),
            _T(""),
            NULL);

        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // Here, we will be using our application's specific layout in the future lab.
        // At the moment, this is a greeting section.
        TextOut(hdc,
            5, 5,
            greeting, _tcslen(greeting));
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case  WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_OPEN:
            MessageBox(hWnd, L"Open file", L"File", MB_OK);
            break;
        case ID_FILE_SAVE:
            MessageBox(hWnd, L"Save file", L"File", MB_OK);
            break;
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;
        case ID_EDIT_CUT:
            MessageBox(hWnd, L"Cut", L"Edit", MB_OK);
            break;
        case ID_EDIT_COPY:
            MessageBox(hWnd, L"Copy", L"Edit", MB_OK);
            break;
        case ID_EDIT_PASTE:
            MessageBox(hWnd, L"Paste", L"Edit", MB_OK);
            break;
        case ID_HELP_ABOUT:
            MessageBox(hWnd, L"About\nYea", L"About", MB_OK);
            break;
        }
        break;
        MessageBox(NULL,
            _T("Call a WM_COMMAND!"),
            _T(""),
            NULL);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T(""),
            NULL);

        return 1;
    }
    HMENU hMainMenu = CreateMenu();

    HMENU hFileMenu = CreatePopupMenu();
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, L"Open");
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, L"Save");
    AppendMenu(hFileMenu, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFileMenu, MF_STRING, ID_FILE_EXIT, L"Exit");

    HMENU hEditMenu = CreatePopupMenu();
    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_CUT, L"Cut");
    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_COPY, L"Copy");
    AppendMenu(hEditMenu, MF_STRING, ID_EDIT_PASTE, L"Paste");

    HMENU hHelpMenu = CreatePopupMenu();
    AppendMenu(hHelpMenu, MF_STRING, ID_HELP_ABOUT, L"About");

    AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
    AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");
    AppendMenu(hMainMenu, MF_POPUP, (UINT_PTR)hHelpMenu, L"Help");

    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        hMainMenu,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T(""),
            NULL);

        return 1;
    }

    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
