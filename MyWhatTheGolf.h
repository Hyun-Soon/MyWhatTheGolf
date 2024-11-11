#pragma once

#include "WindowManager.h"
#include "DirectXManager.h"

class MyWhatTheGolf
{
public:
	MyWhatTheGolf(UINT width, UINT height);
	bool											   Initialize();
	const HWND&										   GetWindow() const;
	const Microsoft::WRL::ComPtr<ID3D11Device>&		   GetDevice() const;
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext() const;
	void											   SwitchBackBuffer();

private:
	Resolution mResolution;
	HWND	   mWindow;
	DX11Data   mDxData;
};