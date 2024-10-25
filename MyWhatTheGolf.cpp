#include "MyWhatTheGolf.h"

MyWhatTheGolf::MyWhatTheGolf(UINT width, UINT height)
	: mResolution(width, height)
{
}

bool MyWhatTheGolf::Initialize()
{
	return (WindowManager::Initialize(mResolution, mWindow) || DirectXManager::Initialize(mResolution, mWindow, mDxData));
}
