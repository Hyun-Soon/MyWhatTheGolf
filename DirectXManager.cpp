#include "DirectXManager.h"

bool DirectXManager::Initialize(_In_ const Resolution& res, _In_ const HWND& window, _Out_ DX11Data& dxData)
{
	// initDirectX
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG) // not executed in release mode
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevels[3] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};
	D3D_FEATURE_LEVEL featureLevel;

	// check if support 4X MSAA
	// UINT numQualityLevels;
	// device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
	// if (numQualityLevels <= 0)
	//{
	//	std::cout << "MSAA not supported." << std::endl;
	// }
	// numQualityLevels = 0; // MSAA turn off

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = res.width;
	swapChainDesc.BufferDesc.Height = res.height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferCount = 2; // Double-buffering
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_SHADER_INPUT : to use input for shader
	swapChainDesc.OutputWindow = window;
	swapChainDesc.Windowed = TRUE;								  // windowed/full-screen mode
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (dxData.numQualityLevel > 0)
	{
		swapChainDesc.SampleDesc.Count = 4; // how many multisamples
		swapChainDesc.SampleDesc.Quality = dxData.numQualityLevel - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}

	if (FAILED(D3D11CreateDeviceAndSwapChain(
			0, // Default adapter
			driverType,
			0, // No software device
			createDeviceFlags,
			featureLevels,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,					 // [in, optional]
			dxData.swapChain.GetAddressOf(), // [out, optional]
			dxData.device.GetAddressOf(),	 // [out, optional]
			&featureLevel,					 // [out, optional]
			dxData.context.GetAddressOf()))) // [out, optional]
	{
		OutputDebugString(L"D3D11CreateDeviceAndSwapChain() failed.\n");
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		OutputDebugString(L"D3D Feature Level 11.1 unsupported.\n");
		return false;
	}

	//  Create RenderTargetView
	dxData.swapChain->GetBuffer(0, IID_PPV_ARGS(dxData.backBuffer.GetAddressOf())); // 0 : index
	if (dxData.backBuffer)
	{
		HRESULT hr = dxData.device->CreateRenderTargetView(dxData.backBuffer.Get(), nullptr, dxData.renderTargetView.GetAddressOf());

		if (FAILED(hr))
			std::cout << "CreateRenderTargetView() failed." << std::endl;
	}
	else
		std::cout << "backBuffer is nullptr." << std::endl;

	// Set Viewport
	ZeroMemory(&dxData.viewport, sizeof(D3D11_VIEWPORT));
	dxData.viewport.TopLeftX = 0;
	dxData.viewport.TopLeftY = 0;
	dxData.viewport.Width = res.width;
	dxData.viewport.Height = res.height;
	dxData.viewport.MinDepth = 0.0f;
	dxData.viewport.MaxDepth = 1.0f; // modify later

	dxData.context->RSSetViewports(1, &dxData.viewport);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK; // D3D11_CULL_BACK
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;

	dxData.device->CreateRasterizerState(&rasterizerDesc, dxData.rasterizerState.GetAddressOf());

	// Create depth buffer
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	depthStencilBufferDesc.Width = res.width;
	depthStencilBufferDesc.Height = res.height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (dxData.numQualityLevel > 0)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = dxData.numQualityLevel - 1;
	}
	else
	{
		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;
	}
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	// depthStencilBuffer
	if (FAILED(dxData.device->CreateTexture2D(&depthStencilBufferDesc, 0, dxData.depthStencilBuffer.GetAddressOf())))
	{
		OutputDebugString(L"CreateTexture2D() failed.\n");
		return false;
	}

	// depthStencilView
	if (FAILED(dxData.device->CreateDepthStencilView(dxData.depthStencilBuffer.Get(), 0, dxData.depthStencilView.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilView() failed.\n");
		return false;
	}

	// Create Depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	if (FAILED(dxData.device->CreateDepthStencilState(&depthStencilDesc, dxData.depthStencilState.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilState() failed.\n");
		return false;
	}

	return true;
}
