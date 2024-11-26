#include "WindowManager.h"
#include <iostream>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// switch (msg)
	//{
	//	case WM_SIZE:
	//		break;
	//	case WM_LBUTTONDOWN:
	//		std::cout << "LBUTTONDOWN" << std::endl;
	//		break;
	// }
	return DefWindowProc(hwnd, msg, wParam, lParam); // Default Window Procedure
}

bool WindowManager::Initialize(_Out_ Resolution& res, _Out_ HWND& window)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), // size of structure
		CS_CLASSDC,						  // style of window
		WndProc,						  // points to window procedure
		0L,
		0L,
		GetModuleHandle(NULL), // because used main function, not WinMain
		NULL,
		NULL,
		NULL,
		NULL,
		L"MyWhatTheGolf", // name of window class
		NULL };

	if (!RegisterClassEx(&wc))
	{
		OutputDebugString(L"RegisterClassEx() failed.\n");
		return 1;
	}

	RECT wr = { 0, 0, res.width, res.height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(wc.lpszClassName,
		L"MyWhatTheGolf",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!window)
	{
		OutputDebugString(L"CreateWindow() failed.\n");
		return false;
	}
	res.width = wr.right - wr.left;
	res.height = wr.bottom - wr.top;

	ShowWindow(window, SW_SHOWDEFAULT);
	// UpdateWindow(mainWindow);
	SetForegroundWindow(window);
	return true;
}