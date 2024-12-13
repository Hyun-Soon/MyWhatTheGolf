#include "MyWhatTheGolf.h"

MyWhatTheGolf::MyWhatTheGolf(UINT width, UINT height)
	: mWndManager(width, height)
{
}

bool MyWhatTheGolf::Initialize()
{
	return (mWndManager.Initialize() && mDxManager.Initialize(mWndManager.GetResolution(), mWndManager.GetWindow()));
}

void MyWhatTheGolf::Run()
{
	// Object golfBat("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/", "golf_bat_bin.fbx", "GradientPalette.png");
	// Object golfBat("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/", "AgarthanBody.fbx"); //, "nemlemtemptex.png");
	// Object golfBat("C:/Users/Soon/Desktop/HonglabGraphics/Part4/Assets/Characters/Mixamo", "character.fbx", "Ch03_1001_Diffuse.png");
	Object swat("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/", "Swat.fbx"); //, "nemlemtemptex.png");

	mDxManager.CreateVertexBuffer(swat);
	mDxManager.CreateIndexBuffer(swat);
	mDxManager.CreateVertexShaderAndInputLayout();
	mDxManager.CreatePixelShader();
	mDxManager.CreateTexture(swat);

	float c = 0.0f;
	float r = 0.0f;

	// edit these lines
	{
		mDxManager.GetContext()->IASetInputLayout(mDxManager.GetLayoutPtr());
		mDxManager.GetContext()->PSSetSamplers(0, 1, mDxManager.GetAddressOfSamplerState());
		mDxManager.GetContext()->PSSetShaderResources(0, 1, swat.GetTextureResourceView().GetAddressOf());
		mDxManager.GetContext()->OMSetRenderTargets(1, mDxManager.GetAddressOfRenderTargetView(), mDxManager.GetDepthStencilView());
		mDxManager.GetContext()->OMSetDepthStencilState(mDxManager.GetDepthStencilState(), 0);
		mDxManager.GetContext()->VSSetShader(mDxManager.GetVertexShader(), 0, 0);
		mDxManager.GetContext()->PSSetShader(mDxManager.GetPixelShader(), 0, 0);
		mDxManager.GetContext()->RSSetState(mDxManager.GetRasterizerState());
	}

	const HWND window = mWndManager.GetWindow();
	MSG		   msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render
			r += 2.0f;
			swat.RotateY(DirectX::XMConvertToRadians(r));

			mDxManager.Render(swat);
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
