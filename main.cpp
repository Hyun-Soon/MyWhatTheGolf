#include "MyWhatTheGolf.h"

#include <vector>
#include <directxtk/SimpleMath.h>
#include <string>
#include <d3dcompiler.h>

struct Vertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 color;
};

struct MeshData
{
	std::vector<Vertex>	  vertices;
	std::vector<uint32_t> indices;
	std::string			  textureFilename;
};

struct VertexConstantData
{
	DirectX::SimpleMath::Matrix model;
	DirectX::SimpleMath::Matrix invTranspose;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(VertexConstantData) % 16) == 0, "Vertex Constant Buffer size must be 16-byte aligned.");

struct PixelConstantData
{
	DirectX::SimpleMath::Vector4 color[3];
	DirectX::SimpleMath::Vector4 padding;
};
static_assert((sizeof(PixelConstantData) % 16 == 0), "Pixel Constant Buffer size must be 16-byte aligned.");

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
	vertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	vertexConstantData.view = DirectX::SimpleMath::Matrix(); // default
	vertexConstantData.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30.0f), float(width) / height, 1.0f, 10.0f);

	/*PixelConstantData pixelConstantData;
	pixelConstantData.color[0] = { 1.0f, 0.0f, 0.0f, 1.0f };
	pixelConstantData.color[1] = { 0.0f, 1.0f, 0.0f, 1.0f };
	pixelConstantData.color[2] = { 0.0f, 0.0f, 1.0f, 1.0f };*/

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
	/*initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;*/

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexConstantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pixelConstantBuffer;

	HRESULT hr = app.GetDevice()->CreateBuffer(&cbDesc, &initData, vertexConstantBuffer.GetAddressOf());

	if (FAILED(hr))
	{
		OutputDebugString(L"CreateConstantBuffer() failed.\n");
		return 1;
	}

	/*initData.pSysMem = &pixelConstantData;
	hr = app.GetDevice()->CreateBuffer(&cbDesc, &initData, pixelConstantBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateConstantBuffer() failed.\n");
		return 2;
	}*/

	// Create Vertex Buffer
	DirectX::SimpleMath::Vector3 vertex1 = { 0.5f, 0.5f, 2.0f };
	DirectX::SimpleMath::Vector3 color1 = { 1.0f, 0.0f, 0.0f };
	DirectX::SimpleMath::Vector3 vertex2 = { -0.5f, 0.5f, 2.0f };
	DirectX::SimpleMath::Vector3 color2 = { 0.0f, 1.0f, 0.0f };
	DirectX::SimpleMath::Vector3 vertex3 = { -0.5f, -0.5f, 2.0f };
	DirectX::SimpleMath::Vector3 color3 = { 0.0f, 0.0f, 1.0f };

	std::vector<Vertex>	  vertices = { { vertex1, color1 }, { vertex2, color2 }, { vertex3, color3 } };
	std::vector<uint32_t> indices = { 0, 1, 2 };

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0; // 0 if no CPU access is neccesary
	vbDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = vertices.data();
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
	ibDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = sizeof(uint32_t);

	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	hr = app.GetDevice()->CreateBuffer(&ibDesc, &indexBufferData, indexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		OutputDebugString(L"CreateBufferBuffer() failed.\n");
		return 3;
	}

	// Create VertexShader and InputLayout
	Microsoft::WRL::ComPtr<ID3DBlob> vertexBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	ID3D11VertexShader*				 vertexShader;
	ID3D11PixelShader*				 pixelShader;
	UINT							 compileFlags = 0;

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

	app.GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);

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

	app.GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	ID3D11InputLayout* layout;
	app.GetDevice()->CreateInputLayout(inputElementDescVector.data(), 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &layout);
	app.GetContext()->IASetInputLayout(layout);

	app.GetContext()->VSSetShader(vertexShader, 0, 0);
	app.GetContext()->PSSetShader(pixelShader, 0, 0);

	// select which vertex buffer to display
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	app.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	app.GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	app.GetContext()->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	app.GetContext()->DrawIndexed(UINT(indices.size()), 0, 0);

	// MSG msg = { 0 };
	// while (WM_QUIT != msg.message)
	//{
	//	PeekMessage(&msg, app.GetWindow(), 0, 0, PM_REMOVE);

	//}
	app.SwitchBackBuffer();

	while (1)
		;

	return 0;
}
