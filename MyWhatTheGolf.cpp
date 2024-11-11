#include "MyWhatTheGolf.h"

MyWhatTheGolf::MyWhatTheGolf(UINT width, UINT height)
	: mResolution(width, height)
{
}

bool MyWhatTheGolf::Initialize()
{
	return (WindowManager::Initialize(mResolution, mWindow) && DirectXManager::Initialize(mResolution, mWindow, mDxData));
}

const HWND& MyWhatTheGolf::GetWindow() const
{
	return this->mWindow;
}

const Microsoft::WRL::ComPtr<ID3D11Device>& MyWhatTheGolf::GetDevice() const
{
	return this->mDxData.device;
}

const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& MyWhatTheGolf::GetContext() const
{
	return this->mDxData.context;
}

void MyWhatTheGolf::SwitchBackBuffer()
{
	mDxData.swapChain->Present(1, 0);
}
