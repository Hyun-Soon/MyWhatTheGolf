#pragma once

#include <d3dcommon.h>
#include <d3dcompiler.h>

#include "WindowManager.h"
#include "Model.h"

class DirectXManager
{
public:
	bool Initialize(_In_ const Resolution& res, _In_ const HWND& window);
	// bool Render(Model& obj);

	// bool CreateVertexBuffer(Model& obj) const;
	// bool CreateIndexBuffer(Model& obj) const;
	// bool CreateTexture(Model& obj) const;
	// bool CreatePixelShader();

	// ID3D11InputLayout*								   GetLayoutPtr() const;
	// ID3D11DepthStencilView*							   GetDepthStencilView();
	// ID3D11DepthStencilState*						   GetDepthStencilState();
	// ID3D11VertexShader*								   GetVertexShader();
	// ID3D11PixelShader*								   GetPixelShader();
	// ID3D11RasterizerState*							   GetRasterizerState();
	// ID3D11SamplerState**							   GetAddressOfSamplerState();
	// ID3D11RenderTargetView**						   GetAddressOfRenderTargetView();
	// const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext() const;

private:
	// bool createRenderTargetView();
	// bool createRasterizerState();
	// bool createDepthStencilBuffer(_In_ const Resolution& res);
	// bool createVertexConstantBuffer();
	// bool createSamplerState();
	// void setViewport(_In_ const Resolution& res);
};