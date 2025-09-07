#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define IDD_ABOUTBOX    101
#define ID_FILE_OPEN    1001
#define ID_FILE_SAVE    1002  
#define ID_FILE_EXIT    1003
#define ID_EDIT_CUT     1004
#define ID_EDIT_COPY    1005
#define ID_EDIT_PASTE   1006
#define ID_HELP_ABOUT   1007
#define IDM_MYMENURESOURCE   3


const int PIPE_SIZE = 20;
int g_numPipesX = 0;
int g_numPipesY = 0;
COLORREF** g_pipeColors = nullptr;

static TCHAR szWindowClass[] = _T("DesktopApp");

static TCHAR szTitle[] = _T("WinApiTextEditor");

HINSTANCE hInst;

HWND g_screensaverWnd = NULL;
bool g_screensaverActive = false;
POINT g_lastScreensaverMousePos;
ULONGLONG g_lastMouseActivity = GetTickCount64();
LRESULT CALLBACK ScreensaverWndProc(HWND, UINT, WPARAM, LPARAM);

void InitializePipes(int width, int height)
{
	if (g_pipeColors)
	{
		for (int y = 0; y < g_numPipesY; ++y)
			delete[] g_pipeColors[y];
		delete[] g_pipeColors;
	}

	g_numPipesX = width / PIPE_SIZE;
	g_numPipesY = height / PIPE_SIZE;

	g_pipeColors = new COLORREF * [g_numPipesY];
	for (int y = 0; y < g_numPipesY; ++y)
	{
		g_pipeColors[y] = new COLORREF[g_numPipesX];
		for (int x = 0; x < g_numPipesX; ++x)
		{
			int r = rand() % 256;
			int g = rand() % 256;
			int b = rand() % 256;
			g_pipeColors[y][x] = RGB(r, g, b);
		}
	}
}

void UpdatePipeColors()
{
	for (int y = 0; y < g_numPipesY; ++y)
	{
		for (int x = 0; x < g_numPipesX; ++x) {
			if (rand() % 50 == 0)
			{
				int r = rand() % 256;
				int g = rand() % 256;
				int b = rand() % 256;
				g_pipeColors[y][x] = RGB(r, g, b);
			}
		}
	}
}

void StartScreensaver(HWND parentWnd)
{
	if (g_screensaverActive) return;

	HWND newWnd = CreateWindowEx(
		0,
		L"ScreensaverClass",
		L"Screensaver",
		WS_POPUP | WS_VISIBLE,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN),  
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if (newWnd) {
		g_screensaverWnd = newWnd;
		g_screensaverActive = true;  
	}
}

void StopScreensaver()
{
	if (!g_screensaverActive) return;

	g_screensaverActive = false;

	if (g_screensaverWnd)
	{
		DestroyWindow(g_screensaverWnd);
		g_screensaverWnd = NULL;
	}

	g_lastMouseActivity = GetTickCount64();
}

void RegisterScreensaverClass(HINSTANCE hInstance)
{
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = ScreensaverWndProc;     // Отдельная процедура
	wc.hInstance = hInstance;
	wc.lpszClassName = L"ScreensaverClass";
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Чёрный фон
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}




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
			MB_OK);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		TextOut(hdc, 5, 5, greeting, _tcslen(greeting));
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
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
			DialogBox(GetModuleHandle(NULL),
				MAKEINTRESOURCE(IDD_ABOUTBOX), 
				hWnd,                           
				AboutDlgProc);                  
			break;
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_NCMOUSEMOVE:
		g_lastMouseActivity = GetTickCount64();
		if (g_screensaverActive) {
			StopScreensaver();
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK ScreensaverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			InitializePipes(clientRect.right, clientRect.bottom);

			// Инициализируем текущую позицию мыши при создании окна
			POINT currentPos;
			GetCursorPos(&currentPos);
			g_lastScreensaverMousePos = currentPos; // Глобальная переменная

			SetTimer(hWnd, 1, 100, NULL);
		}
		break;

	case WM_TIMER:
		UpdatePipeColors();
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		for (int y = 0; y < g_numPipesY; ++y)
		{
			for (int x = 0; x < g_numPipesX; ++x)
			{
				HBRUSH brush = CreateSolidBrush(g_pipeColors[y][x]);

				RECT pipeRect = {
					x * PIPE_SIZE,           // left
					y * PIPE_SIZE,           // top  
					(x + 1) * PIPE_SIZE,     // right
					(y + 1) * PIPE_SIZE      // bottom
				};

				FillRect(hdc, &pipeRect, brush);

				DeleteObject(brush);
			}
		}

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_SIZE:
		InitializePipes(LOWORD(lParam), HIWORD(lParam));
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
	{
		POINT currentPos;
		GetCursorPos(&currentPos);

		if (g_lastScreensaverMousePos.x != currentPos.x ||
			g_lastScreensaverMousePos.y != currentPos.y) {
			StopScreensaver();
		}
	}
	break;

	case WM_LBUTTONDOWN: 
	case WM_KEYDOWN:
		StopScreensaver();
		break;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		if (g_pipeColors)
		{
			for (int y = 0; y < g_numPipesY; ++y)
				delete[] g_pipeColors[y];
			delete[] g_pipeColors;
			g_pipeColors = nullptr;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
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
	RegisterScreensaverClass(hInstance);


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
		500, 500,
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


	g_lastMouseActivity = GetTickCount64();

	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	bool running = true;
	while (running)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				running = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		if (!g_screensaverActive)
		{
			ULONGLONG currentTime = GetTickCount64();
			ULONGLONG elapsed = currentTime - g_lastMouseActivity;

			if (elapsed >= 30000) // 30 seconds to mars
			{
				StartScreensaver(hWnd);
			}
		}


		Sleep(50);
	}

	return (int)msg.wParam;
}

