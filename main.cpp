#include "MyWhatTheGolf.h"

#include <vector>
#include <directxtk/SimpleMath.h>
#include <string>
#include <d3dcompiler.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
	vertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(0.0f, 30.0f, 60.0f); // * DirectX::SimpleMath::Matrix::CreateRotationZ(DirectX::XMConvertToRadians(90.0f));

	//RENDERED
	vertexConstantData.view = DirectX::XMMatrixLookToLH(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));

	//RENDERED
	//vertexConstantData.view = DirectX::XMMatrixLookAtLH(
	//	DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
	//	DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
	//	DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));

	//NOT RENDERED
	//vertexConstantData.view = DirectX::SimpleMath::Matrix::CreateLookAt(
	//	DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
	//	DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
	//	DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));


	vertexConstantData.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), float(width) / height, 1.0f, 100.0f);

	vertexConstantData.model = vertexConstantData.model.Transpose();
	vertexConstantData.view = vertexConstantData.view.Transpose();
	vertexConstantData.projection = vertexConstantData.projection.Transpose();

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



	////// Create Vertex Buffer
	//DirectX::SimpleMath::Vector3 vertex1 = { -50.0f, -50.0f, -50.0f };
	////DirectX::SimpleMath::Vector3 vertex1 = { -0.3f, -0.3f, -1.0f };
	//DirectX::SimpleMath::Vector3 color1 = { 1.0f, 0.0f, 0.0f }; // r

	//DirectX::SimpleMath::Vector3 vertex2 = { 50.0f, 50.0f, -50.0f };
	////DirectX::SimpleMath::Vector3 vertex2 = { -0.3f, 0.3f, -1.0f };
	//DirectX::SimpleMath::Vector3 color2 = { 0.0f, 1.0f, 0.0f }; // g

	//DirectX::SimpleMath::Vector3 vertex3 = { -50.0f, 50.0f, -50.0f };
	////DirectX::SimpleMath::Vector3 vertex3 = { 0.3f, 0.3f, -1.0f };
	//DirectX::SimpleMath::Vector3 color3 = { 0.0f, 0.0f, 1.0f }; // b

	//DirectX::SimpleMath::Vector3 vertex4 = { 50.0f, -50.0f, -50.0f };
	//DirectX::SimpleMath::Vector3 color4 = { 1.0f, 1.0f, 1.0f }; // w

	//std::vector<Vertex>	  vertices = { { vertex1, color1 }, { vertex2, color2 }, { vertex3, color3 }, { vertex4, color4 } };
	//std::vector<uint32_t> indices = { 0, 1, 2, 0, 4, 1};

	Assimp::Importer importer;
	const aiScene*	 scene = importer.ReadFile("C:/Users/Soon/Desktop/MyWhatTheGolf/Assets/golf_bat_bin.fbx", aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
		return -1;
	}

	std::vector<DirectX::SimpleMath::Vector3> vertices;
	//std::vector<DirectX::XMFLOAT3> normals;
	//std::vector<DirectX::XMFLOAT2> uvs;
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{
			aiVector3D pos = mesh->mVertices[j];
			vertices.push_back(DirectX::SimpleMath::Vector3(pos.x, pos.y, pos.z));
			
			//if (mesh->HasNormals())
			//{
			//	aiVector3D normal = mesh->mNormals[j];
			//	normals.push_back(DirectX::XMFLOAT3(normal.x, normal.y, normal.z));
			//}

			//if (mesh->mTextureCoords[0])
			//{
			//	aiVector3D texCoord = mesh->mTextureCoords[0][j];
			//	uvs.push_back(DirectX::XMFLOAT2(texCoord.x, texCoord.y));
			//}
		} 

		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				indices.push_back(face.mIndices[k]);
			}
		}
	}

	////debug
	//std::cout << "size : " << vertices.size() << ", " << indices.size() << '\n';
	//for (size_t i = 0; i < vertices.size(); i++)
	//{
	//	std::cout << "index : " << i << "-> (" << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << ")\n";
	//}

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbDesc.ByteWidth = static_cast<UINT>(sizeof(DirectX::SimpleMath::Vector3) * vertices.size());
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0; // 0 if no CPU access is neccesary
	vbDesc.StructureByteStride = sizeof(DirectX::SimpleMath::Vector3);

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

	/*std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};*/

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescVector = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
			UINT stride = sizeof(DirectX::SimpleMath::Vector3);
			UINT offset = 0;

			app.GetContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
			app.GetContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			app.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			app.GetContext()->DrawIndexed(UINT(indices.size()), 0, 0);
			app.SwitchBackBuffer();
		}
	}

	// while (1)
	//	;

	return 0;
}
