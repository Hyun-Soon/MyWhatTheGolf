#pragma once

#include <d3dcommon.h>
#include <d3dcompiler.h>

#include "WindowManager.h"
#include "Object.h"

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
	Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>  layout;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;

	D3D11_VIEWPORT		viewport;
	VertexConstantData* pVertexConstantData;
};

class DirectXManager
{
public:
	bool Initialize(_In_ const Resolution& res, _In_ const HWND& window);
	bool CreateDeviceContextAndSwapChain();
	bool CreateVertexBuffer(Object& obj) const;
	bool CreateIndexBuffer(Object& obj) const;
	bool CreateVertexShaderAndInputLayout();
	bool CreatePixelShader();
	bool Render(Object& obj);

	// private:
	DX11Data mDxData;
};