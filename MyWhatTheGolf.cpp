#include "MyWhatTheGolf.h"

MyWhatTheGolf::MyWhatTheGolf(UINT width, UINT height)
	: mResolution(width, height)
{
}

bool MyWhatTheGolf::Initialize()
{
	return (WindowManager::Initialize(mResolution, mWindow) && mDxManager.Initialize(mResolution, mWindow));
}

void MyWhatTheGolf::Run()
{
	Object golfBat("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/", "golf_bat_bin.fbx");
	mDxManager.CreateVertexBuffer(golfBat);
	mDxManager.CreateIndexBuffer(golfBat);
	mDxManager.CreateVertexShaderAndInputLayout();
	mDxManager.CreatePixelShader();

	float c = 0.0f;
	float r = 0.0f;

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, mWindow, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render
			c += 0.01f;

			if (c < 5.0f)
				golfBat.Transform({ 0.0f, 0.1f * sin(4.0f * c), 0.0f });
			else if (c <= 5.5f)
			{
				r -= 2.0f;
				golfBat.RotateX(DirectX::XMConvertToRadians(r));
			}
			else
			{
			}
			mDxManager.Render(golfBat);
		}
	}
}

// const HWND& MyWhatTheGolf::GetWindow() const
//{
//	return this->mWindow;
// }

// const Microsoft::WRL::ComPtr<ID3D11Device>& MyWhatTheGolf::GetDevice() const
//{
//	return this->mDxData.device;
// }
//
// const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& MyWhatTheGolf::GetContext() const
//{
//	return this->mDxData.context;
// }
//
// void MyWhatTheGolf::SwitchBackBuffer()
//{
//	mDxData.swapChain->Present(1, 0);
// }
//
// void MyWhatTheGolf::Render()
//{
// }
//
// DX11Data& MyWhatTheGolf::getDxData()
//{
//	return mDxData;
// }
