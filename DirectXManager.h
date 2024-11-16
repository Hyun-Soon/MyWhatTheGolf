#pragma once

#include <d3dcommon.h>
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include "WindowManager.h"

// debug
#include <iostream>

struct DX11Data
{
	UINT numQualityLevel = 0;

	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;

	D3D11_VIEWPORT viewport;

};

class DirectXManager
{
public:
	static bool Initialize(_In_ const Resolution& res, _In_ const HWND& window, _Out_ DX11Data& dxData);
};