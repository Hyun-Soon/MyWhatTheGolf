#include "MyWhatTheGolf.h"
#include "AssetLoader.h"
#include "Object.h"

#include <d3dcompiler.h>
#include <algorithm>


// TODO :: 1. SimpleMath -> XM



//struct MeshData
//{
//	std::vector<Vertex>	  vertices;
//	std::vector<uint32_t> indices;
//	std::string			  textureFilename;
//};



//struct PixelConstantData
//{
//	DirectX::SimpleMath::Vector4 color[3];
//	DirectX::SimpleMath::Vector4 padding;
//};
//static_assert((sizeof(PixelConstantData) % 16 == 0), "Pixel Constant Buffer size must be 16-byte aligned.");

// 디버깅 위해서 main 함수 사용. 위 함수 사용하려면 sln -> properties -> Linker -> System -> Subsystem -> Windows로 설정
// int main()
// int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE, char*, int)

// int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
int main()
{
	const int	  width = 1280;
	const int	  height = 960;
	MyWhatTheGolf app(width, height);

	if (!app.Initialize())
		return 1;

	VertexConstantData vertexConstantData;
	vertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, 30.0f, 60.0f); //DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(120.0f)) * 

	//Vertex Constant Data
	vertexConstantData.view = DirectX::XMMatrixLookToLH(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));

	vertexConstantData.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), float(width) / height, 1.0f, 100.0f);

	vertexConstantData.model = vertexConstantData.model.Transpose();
	vertexConstantData.view = vertexConstantData.view.Transpose();
	vertexConstantData.projection = vertexConstantData.projection.Transpose();

	// Create Constant Buffer
	D3D11_BUFFER_DESC cbDesc;
	ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
	cbDesc.ByteWidth = sizeof(vertexConstantData);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = &vertexConstantData;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexConstantBuffer;
	// Microsoft::WRL::ComPtr<ID3D11Buffer> pixelConstantBuffer;

	HRESULT hr = app.GetDevice()->CreateBuffer(&cbDesc, &initData, vertexConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugString(L"CreateConstantBuffer() failed.\n");
		return 1;
	}

	// Update Constant Buffer
	D3D11_MAPPED_SUBRESOURCE ms;
	app.GetContext()->Map(vertexConstantBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
	memcpy(ms.pData, &vertexConstantData, sizeof(vertexConstantData)); // ms.pData : GPU's memory that CPU can access because it's mapped
	app.GetContext()->Unmap(vertexConstantBuffer.Get(), NULL);


	Object golfBat;
	AssetLoader::Load("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/", "golf_bat_bin.fbx", golfBat);

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * golfBat.vertices.size());
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0; // 0 if no CPU access is neccesary
	vbDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = golfBat.vertices.data();
	vertexBufferData.SysMemPitch = 0;	   // used for 2D or 3D texture
	vertexBufferData.SysMemSlicePitch = 0; // used for 3D texture

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	hr = app.GetDevice()->CreateBuffer(&vbDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateBuffer() failed.\n");
		return 2;
	}

	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
	ibDesc.ByteWidth = UINT(sizeof(uint32_t) * golfBat.indices.size());
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = golfBat.indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = sizeof(uint32_t);

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	hr = app.GetDevice()->CreateBuffer(&ibDesc, &indexBufferData, indexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateBufferBuffer() failed.\n");
		return 3;
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Create VertexShader and InputLayout
	Microsoft::WRL::ComPtr<ID3DBlob>		   vertexBlob;
	Microsoft::WRL::ComPtr<ID3DBlob>		   pixelBlob;
	Microsoft::WRL::ComPtr<ID3DBlob>		   errorBlob;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader;
	UINT									   compileFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompileFromFile(L"VertexShader.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, &vertexBlob, &errorBlob);

	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			OutputDebugString(L"File not found.\n");
			return 8;
		}

		if (errorBlob)
		{
			OutputDebugString(L"Shader compile error.\n");
			OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
			return 9;
		}
	}

	app.GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, vertexShader.GetAddressOf());

	// Create Input Layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> layout;
	app.GetDevice()->CreateInputLayout(inputElementDescVector.data(), UINT(inputElementDescVector.size()), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), layout.GetAddressOf());

	// Create Pixel Shader
	hr = D3DCompileFromFile(L"PixelShader.hlsl", 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compileFlags, 0, &pixelBlob, &errorBlob);

	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			OutputDebugString(L"File not found.\n");
			return 10;
		}

		if (errorBlob)
		{
			OutputDebugString(L"Shader compile error.\n");
			OutputDebugString((LPCWSTR)errorBlob->GetBufferPointer());
			return 11;
		}
	}

	app.GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, pixelShader.GetAddressOf());

	app.GetContext()->IASetInputLayout(layout.Get());

	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, app.GetWindow(), 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Render
			DX11Data& dxData = app.getDxData();
			float	  clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			app.GetContext()->ClearRenderTargetView(dxData.renderTargetView.Get(), clearColor);
			app.GetContext()->ClearDepthStencilView(dxData.depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

			app.GetContext()->OMSetRenderTargets(1, dxData.renderTargetView.GetAddressOf(), dxData.depthStencilView.Get());
			app.GetContext()->OMSetDepthStencilState(dxData.depthStencilState.Get(), 0);

			app.GetContext()->RSSetState(dxData.rasterizerState.Get());

			app.GetContext()->VSSetShader(vertexShader.Get(), 0, 0);
			app.GetContext()->PSSetShader(pixelShader.Get(), 0, 0);

			app.GetContext()->VSSetConstantBuffers(0, 1, vertexConstantBuffer.GetAddressOf());

			// select which vertex buffer to display
			UINT stride = sizeof(Vertex);
			UINT offset = 0;

			app.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			app.GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			app.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			app.GetContext()->DrawIndexed(UINT(golfBat.indices.size()), 0, 0);
			app.SwitchBackBuffer();
		}
	}

	// while (1)
	//	;

	return 0;
}
