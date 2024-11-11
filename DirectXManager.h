#pragma once
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
	Microsoft::WRL::ComPtr<ID3D11Device>		device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		swapChain;
};

class DirectXManager
{
public:
	static bool Initialize(_In_ const Resolution& res, _In_ const HWND& window, _Out_ DX11Data& dxData);
};