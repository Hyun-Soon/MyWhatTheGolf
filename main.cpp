#include <iostream>
#include <Windows.h>
#include <d3dcommon.h>
#include <d3d11.h>

#include <wrl.h>

const UINT width = 1200;
const UINT height = 800;

LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// switch (msg)
	//{
	//	case WM_SIZE:
	//		break;
	//	case WM_LBUTTONDOWN:
	//		std::cout << "LBUTTONDOWN" << std::endl;
	//		break;
	// }
	return DefWindowProc(hwnd, msg, wParam, lParam); // Default Window Procedure
}

// int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, char*, int)
// 디버깅 위해서 main 함수 사용. 위 함수 사용하려면 sln -> properties -> System -> Subsystem -> Windows로 설정
int main()
{
	// MessageBox(0, L"message", L"title", MB_OK);
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), // size of structure
		CS_CLASSDC,						  // style of window
		WndProc,						  // points to window procedure
		0L,
		0L,
		GetModuleHandle(NULL), // because used main function, not WinMain
		NULL,
		NULL,
		NULL,
		NULL,
		L"MyWhatTheGolf", // name of window class
		NULL };

	if (!RegisterClassEx(&wc))
	{
		std::cout << "RegisterClassEx() failed." << std::endl;
		return 1;
	}

	RECT wr = { 0, 0, width, height };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	HWND mainWindow = CreateWindow(wc.lpszClassName,
		L"MyWhatTheGolf",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!mainWindow)
	{
		std::cout << "CreateWindow() failed." << std::endl;
		return 2;
	}

	ShowWindow(mainWindow, SW_SHOWDEFAULT);
	// UpdateWindow(mainWindow); // I don't know this line is necessary.

	// initDirectX
	const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	UINT createDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG) // not executed in release mode
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	Microsoft::WRL::ComPtr<ID3D11Device>		device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>		swapChain;

	const D3D_FEATURE_LEVEL featureLevels[3] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_9_3
	};
	D3D_FEATURE_LEVEL featureLevel;

	// check if support 4X MSAA
	UINT numQualityLevels;
	// device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
	// if (numQualityLevels <= 0)
	//{
	//	std::cout << "MSAA not supported." << std::endl;
	// }
	numQualityLevels = 0; // MSAA turn off

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferCount = 2; // Double-buffering
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // DXGI_USAGE_SHADER_INPUT : to use input for shader
	sd.OutputWindow = mainWindow;
	sd.Windowed = TRUE;								   // windowed/full-screen mode
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	if (numQualityLevels > 0)
	{
		sd.SampleDesc.Count = 4; // how many multisamples
		sd.SampleDesc.Quality = numQualityLevels - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	if (FAILED(D3D11CreateDeviceAndSwapChain(
			0, // Default adapter
			driverType,
			0, // No software device
			createDeviceFlags,
			featureLevels,
			1,
			D3D11_SDK_VERSION,
			&sd,					  // [in, optional]
			swapChain.GetAddressOf(), // [out, optional]
			device.GetAddressOf(),	  // [out, optional]
			&featureLevel,			  // [out, optional]
			context.GetAddressOf()))) // [out, optional]
	{
		std::cout << "D3D11CreateDeviceAndSwapChain() failed." << std::endl;
		return 3;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		std::cout << "D3D Feature Level 11.1 unsupported." << std::endl;
		return 4;
	}

	// Create RenderTargetView
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   backBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())); // 0 : index
	if (backBuffer)
	{
		device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTargetView.GetAddressOf());

		// D3D11_TEXTURE2D_DESC td;
		// backBuffer->GetDesc(&td);
		// td.SampleDesc.Count = 1;
		// td.SampleDesc.Quality = 0;
		// td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		// td.MiscFlags = 0;
	}

	// Set Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f; // modify later

	context->RSSetViewports(1, &viewport);

	while (1)
		;

	return 0;
}