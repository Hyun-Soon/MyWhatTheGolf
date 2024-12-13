#include "DirectXManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool DirectXManager::Initialize(_In_ const Resolution& res, _In_ const HWND& window)
{
	if (!createDeviceContextAndSwapChain(res, window))
		return false;

	if (!createRenderTargetView())
		return false;

	if (!createRasterizerState())
		return false;

	if (!createDepthStencilBuffer(res))
		return false;

	if (!createVertexConstantBuffer())
		return false;

	if (!createSamplerState())
		return false;

	setViewport(res);

	return true;
}

bool DirectXManager::CreateVertexBuffer(Object& obj) const
{
	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = static_cast<UINT>(sizeof(VERTEX_TYPE) * obj.GetVSize());
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0; // 0 if no CPU access is neccesary
	vbDesc.StructureByteStride = sizeof(VERTEX_TYPE);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = obj.GetVData();
	vertexBufferData.SysMemPitch = 0;	   // used for 2D or 3D texture
	vertexBufferData.SysMemSlicePitch = 0; // used for 3D texture

	HRESULT hr = mDxData.device->CreateBuffer(&vbDesc, &vertexBufferData, obj.GetVertexBuffer().GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateBuffer() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::CreateIndexBuffer(Object& obj) const
{
	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.ByteWidth = UINT(sizeof(uint32_t) * obj.GetISize());
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = obj.GetIData();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = sizeof(uint32_t);

	HRESULT hr = mDxData.device->CreateBuffer(&ibDesc, &indexBufferData, obj.GetIndexBuffer().GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateBufferBuffer() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::CreateTexture(Object& obj) const
{
	int width, height, channels;

	unsigned char* img =
		stbi_load(obj.GetCstyleTextureFilename(), &width, &height, &channels, 0);

	assert(width * height <= INT_MAX);

	// 4 channel
	std::vector<uint8_t> image(width * height * 4);
	for (size_t i = 0; i < width * height; i++)
	{
		for (size_t c = 0; c < 3; c++)
		{
			image[4 * i + c] = img[i * channels + c];
		}
		image[4 * i + 3] = 255;
	}

	D3D11_TEXTURE2D_DESC txtDesc = { 0 };
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = txtDesc.ArraySize = 1;
	txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData = { 0 };
	InitData.pSysMem = image.data();
	InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * 4;
	// InitData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  texture = obj.GetTexture();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& trv = obj.GetTextureResourceView();

	mDxData.device->CreateTexture2D(&txtDesc, &InitData, texture.GetAddressOf());
	mDxData.device->CreateShaderResourceView(texture.Get(), nullptr, trv.GetAddressOf());

	return true;
}

bool DirectXManager::CreateVertexShaderAndInputLayout()
{
	/*std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 * 2, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};*/

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 1, DXGI_FORMAT_R8G8B8A8_UINT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	UINT							 compileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, &vertexBlob, &errorBlob);

	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			OutputDebugString(L"File not found.\n");
			return false;
		}

		if (errorBlob)
		{
			OutputDebugString(L"Shader compile error.\n");
			OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
			return false;
		}
	}

	hr = mDxData.device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, mDxData.vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateVertexShader() failed.\n");
		return false;
	}

	hr = mDxData.device->CreateInputLayout(inputElementDescVector.data(), UINT(inputElementDescVector.size()), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), mDxData.layout.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateInputLayout() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::CreatePixelShader()
{
	Microsoft::WRL::ComPtr<ID3DBlob> pixelBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// Create Pixel Shader
	HRESULT hr = D3DCompileFromFile(L"PixelShader.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compileFlags, 0, &pixelBlob, &errorBlob);

	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			OutputDebugString(L"File not found.\n");
			return false;
		}

		if (errorBlob)
		{
			OutputDebugString(L"Shader compile error.\n");
			OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
			return false;
		}
	}

	hr = mDxData.device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, mDxData.pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreatePixelShader() failed.\n");
		return false;
	}
	return true;
}

ID3D11InputLayout* DirectXManager::GetLayoutPtr() const
{
	return this->mDxData.layout.Get();
}

ID3D11SamplerState** DirectXManager::GetAddressOfSamplerState()
{
	return this->mDxData.samplerState.GetAddressOf();
}

ID3D11RenderTargetView** DirectXManager::GetAddressOfRenderTargetView()
{
	return this->mDxData.renderTargetView.GetAddressOf();
}

ID3D11DepthStencilView* DirectXManager::GetDepthStencilView()
{
	return this->mDxData.depthStencilView.Get();
}

ID3D11DepthStencilState* DirectXManager::GetDepthStencilState()
{
	return this->mDxData.depthStencilState.Get();
}

ID3D11VertexShader* DirectXManager::GetVertexShader()
{
	return this->mDxData.vertexShader.Get();
}

ID3D11PixelShader* DirectXManager::GetPixelShader()
{
	return this->mDxData.pixelShader.Get();
}

ID3D11RasterizerState* DirectXManager::GetRasterizerState()
{
	return this->mDxData.rasterizerState.Get();
}

const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& DirectXManager::GetContext() const
{
	return this->mDxData.context;
}

bool DirectXManager::Render(Object& obj)
{
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context = mDxData.context;
	float										 clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	context->ClearRenderTargetView(mDxData.renderTargetView.Get(), clearColor);
	context->ClearDepthStencilView(mDxData.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	mDxData.pVertexConstantData = obj.GetPVertexConstantData();

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mDxData.vertexConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, mDxData.pVertexConstantData, sizeof(VERTEX_CONSTANT_DATA_TYPE));
	context->Unmap(mDxData.vertexConstantBuffer.Get(), NULL);
	context->VSSetConstantBuffers(0, 1, mDxData.vertexConstantBuffer.GetAddressOf());

	//// select which vertex buffer to display
	UINT stride = sizeof(VERTEX_TYPE);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, obj.GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(obj.GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(UINT(obj.GetISize()), 0, 0);
	mDxData.swapChain->Present(1, 0);

	return true;
}

bool DirectXManager::createDeviceContextAndSwapChain(_In_ const Resolution& res, _In_ const HWND& window)
{
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

	if (mDxData.numQualityLevel > 0)
	{
		swapChainDesc.SampleDesc.Count = 4; // how many multisamples
		swapChainDesc.SampleDesc.Quality = mDxData.numQualityLevel - 1;
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
			&swapChainDesc,					  // [in, optional]
			mDxData.swapChain.GetAddressOf(), // [out, optional]
			mDxData.device.GetAddressOf(),	  // [out, optional]
			&mFeatureLevel,					  // [out, optional]
			mDxData.context.GetAddressOf()))) // [out, optional]
	{
		OutputDebugString(L"D3D11CreateDeviceAndSwapChain() failed.\n");
		return false;
	}

	if (mFeatureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		OutputDebugString(L"D3D Feature Level 11.1 unsupported.\n");
		return false;
	}
	return true;
}

bool DirectXManager::createRenderTargetView()
{
	mDxData.swapChain->GetBuffer(0, IID_PPV_ARGS(mDxData.backBuffer.GetAddressOf())); // 0 : index
	if (mDxData.backBuffer)
	{
		HRESULT hr = mDxData.device->CreateRenderTargetView(mDxData.backBuffer.Get(), nullptr, mDxData.renderTargetView.GetAddressOf());

		if (FAILED(hr))
		{
			OutputDebugString(L"CreateRenderTargetView() failed.\n");
			return false;
		}
	}
	else
	{
		OutputDebugString(L"backBuffer is nullptr.\n");
		return false;
	}
	return true;
}

bool DirectXManager::createRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID; // D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthClipEnable = TRUE;

	HRESULT hr = mDxData.device->CreateRasterizerState(&rasterizerDesc, mDxData.rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateRasterizerState() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::createDepthStencilBuffer(_In_ const Resolution& res)
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = { 0 };
	depthStencilBufferDesc.Width = res.width;
	depthStencilBufferDesc.Height = res.height;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits for depth, 8 bits for stencil
	if (mDxData.numQualityLevel > 0)
	{
		depthStencilBufferDesc.SampleDesc.Count = 4;
		depthStencilBufferDesc.SampleDesc.Quality = mDxData.numQualityLevel - 1;
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

	// Create Depth Stencil Buffer
	if (FAILED(mDxData.device->CreateTexture2D(&depthStencilBufferDesc, 0, mDxData.depthStencilBuffer.GetAddressOf())))
	{
		OutputDebugString(L"CreateTexture2D() failed.\n");
		return false;
	}

	// Create Depth Stencil View
	if (FAILED(mDxData.device->CreateDepthStencilView(mDxData.depthStencilBuffer.Get(), 0, mDxData.depthStencilView.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilView() failed.\n");
		return false;
	}

	// Create Depth Stencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

	if (FAILED(mDxData.device->CreateDepthStencilState(&depthStencilDesc, mDxData.depthStencilState.GetAddressOf())))
	{
		OutputDebugString(L"CreateDepthStencilState() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::createVertexConstantBuffer()
{
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(VERTEX_CONSTANT_DATA_TYPE);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	VertexConstantData	   temp;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = &temp;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	HRESULT hr = mDxData.device->CreateBuffer(&cbDesc, &initData, mDxData.vertexConstantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateConstantBuffer() failed.\n");
		return false;
	}
	return true;
}

bool DirectXManager::createSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the Sample State
	HRESULT hr = mDxData.device->CreateSamplerState(&sampDesc, mDxData.samplerState.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateSamplerState() failed.\n");
		return false;
	}
	return true;
}

void DirectXManager::setViewport(_In_ const Resolution& res)
{
	ZeroMemory(&mDxData.viewport, sizeof(D3D11_VIEWPORT));
	mDxData.viewport.TopLeftX = 0;
	mDxData.viewport.TopLeftY = 0;
	mDxData.viewport.Width = res.width;
	mDxData.viewport.Height = res.height;
	mDxData.viewport.MinDepth = 0.0f;
	mDxData.viewport.MaxDepth = 1.0f;

	mDxData.context->RSSetViewports(1, &mDxData.viewport);
}
