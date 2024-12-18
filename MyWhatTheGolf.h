#pragma once

#include "WindowManager.h"
#include "DirectXManager.h"

class MyWhatTheGolf
{
public:
	MyWhatTheGolf(UINT width, UINT height);

	bool Initialize();
	bool InitializeWindow();
	bool InitializeDirectX();

	void Run();

private:
	bool createDeviceContextAndSwapChain();
	bool createRenderTargetView();
	bool createRasterizerState();
	bool createDepthStencilBuffer();

	struct Resolution
	{
		UINT width;
		UINT height;
		Resolution(UINT w, UINT h) { this->width = w, this->height = h; }
	};

	Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		mSwapChain;
	HWND										mWindow;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mBackBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mVertexConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>		mSamplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  mLayout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  mPixelShader;

	D3D11_VIEWPORT		viewport;
	VertexConstantData* pVertexConstantData;

	UINT			  mNumQualityLevel = 0;
	Resolution		  mResolution;
	D3D_FEATURE_LEVEL mFeatureLevel;
	DirectXManager	  mDxManager;

	std::vector<Object> mObjectList;
};