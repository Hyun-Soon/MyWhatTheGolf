#pragma once

#include "WindowManager.h"
#include "DirectXManager.h"

class MyWhatTheGolf
{
public:
	MyWhatTheGolf(UINT width, UINT height);
	bool Initialize();
	void Run();

private:
	WindowManager  mWndManager;
	DirectXManager mDxManager;
};