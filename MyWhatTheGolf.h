#pragma once

#include "WindowManager.h"
#include "DirectXManager.h"

class MyWhatTheGolf
{
public:
	MyWhatTheGolf(UINT width, UINT height);
	bool Initialize();

private:
	Resolution mResolution;
	HWND	   mWindow;
	DX11Data   mDxData;
};