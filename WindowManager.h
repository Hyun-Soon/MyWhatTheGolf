#pragma once

#include <Windows.h>

struct Resolution
{
	UINT width;
	UINT height;
	Resolution(UINT w, UINT h) { this->width = w, this->height = h; }
};

class WindowManager
{
public:
	static bool Initialize(_In_ const Resolution res, _Out_ HWND& window);

private:
};