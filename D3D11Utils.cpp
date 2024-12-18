// #define _CRT_SECURE_NO_WARNINGS // stb_image_write compile error fix

#include "D3D11Utils.h"

#include <DirectXTexEXR.h> // EXR ���� HDRI �б�
#include <algorithm>
#include <cctype>
#include <directxtk/DDSTextureLoader.h> // ť��� ���� �� �ʿ�
#include <dxgi.h>						// DXGIFactory
#include <dxgi1_4.h>					// DXGIFactory4
#include <execution>
#include <fp16.h>
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

bool D3D11Utils::CheckCreateShaderResult(HRESULT hr, ID3DBlob* errorBlob)
{
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
	return true;
}

bool D3D11Utils::CheckResult(HRESULT hr)
{
	if (FAILED(hr))
	{
		if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
		{
			OutputDebugString(L"File not found.\n");
		}
		return false;
	}
	return true;
}

void D3D11Utils::CreateVertexShaderAndInputLayout(
	Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
	std::wstring								 filename,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
	Microsoft::WRL::ComPtr<ID3D11VertexShader>&	 m_vertexShader,
	Microsoft::WRL::ComPtr<ID3D11InputLayout>&	 m_inputLayout,
	const std::vector<D3D_SHADER_MACRO>			 shaderMacros)
{

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT							 hr = D3DCompileFromFile(
		 filename.c_str(), shaderMacros.empty() ? NULL : shaderMacros.data(),
		 D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0,
		 &shaderBlob, &errorBlob);
	CheckCreateShaderResult(hr, errorBlob.Get());

	/*
		// .cso ������ �о���̴� ���, D3DReadFileToBlob() ���
		std::wstring path = L"x64/Release/";
	#if defined(DEBUG) || defined(_DEBUG)
		path = L"x64/Debug/";
	#endif
		filename.erase(filename.end() - 4, filename.end()); // Ȯ���� hlsl ����
		filename = path + filename + L"cso";
		HRESULT hr = D3DReadFileToBlob(filename.c_str(),
	shaderBlob.GetAddressOf());
	*/

	device->CreateVertexShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&m_vertexShader);

	device->CreateInputLayout(inputElements.data(), UINT(inputElements.size()),
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), &m_inputLayout);
}

void D3D11Utils::CreateHullShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const std::wstring&													filename,
	Microsoft::WRL::ComPtr<ID3D11HullShader>&							m_hullShader)
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���̴��� �������� �̸��� "main"�� �Լ��� ����
	// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
	HRESULT hr = D3DCompileFromFile(
		filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		"hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckCreateShaderResult(hr, errorBlob.Get());

	device->CreateHullShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL, &m_hullShader);
}

void D3D11Utils::CreateDomainShader(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
	Microsoft::WRL::ComPtr<ID3D11DomainShader>& m_domainShader)
{

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���̴��� �������� �̸��� "main"�� �Լ��� ����
	// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
	HRESULT hr = D3DCompileFromFile(
		filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		"ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckCreateShaderResult(hr, errorBlob.Get());

	device->CreateDomainShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&m_domainShader);
}

void D3D11Utils::CreatePixelShader(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const std::wstring&													 filename,
	Microsoft::WRL::ComPtr<ID3D11PixelShader>&							 m_pixelShader)
{
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���̴��� �������� �̸��� "main"�� �Լ��� ����
	// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
	HRESULT hr = D3DCompileFromFile(
		filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		"ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckCreateShaderResult(hr, errorBlob.Get());

	device->CreatePixelShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&m_pixelShader);
}

void D3D11Utils::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const std::vector<uint32_t>&										 indices,
	Microsoft::WRL::ComPtr<ID3D11Buffer>&								 indexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // �ʱ�ȭ �� ����X
	bufferDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
	bufferDesc.StructureByteStride = sizeof(uint32_t);

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	device->CreateBuffer(&bufferDesc, &indexBufferData,
		indexBuffer.GetAddressOf());
}

void D3D11Utils::CreateGeometryShader(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader)
{

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���̴��� �������� �̸��� "main"�� �Լ��� ����
	// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
	HRESULT hr = D3DCompileFromFile(
		filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		"gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	// CheckResult(hr, errorBlob.Get());

	device->CreateGeometryShader(shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(), NULL,
		&geometryShader);
}

void D3D11Utils::CreateComputeShader(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, const std::wstring& filename,
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>& computeShader)
{

	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// ���̴��� �������� �̸��� "main"�� �Լ��� ����
	// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
	HRESULT hr = D3DCompileFromFile(
		filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main",
		"cs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

	CheckCreateShaderResult(hr, errorBlob.Get());

	if (CheckResult(device->CreateComputeShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &computeShader)))
		; // return false;
}

// void ReadEXRImage(const std::string filename, std::vector<uint8_t>& image,
//	int& width, int& height, DXGI_FORMAT& pixelFormat)
//{
//
//	const std::wstring wFilename(filename.begin(), filename.end());
//
//	TexMetadata metadata;
//	ThrowIfFailed(GetMetadataFromEXRFile(wFilename.c_str(), metadata));
//
//	ScratchImage scratchImage;
//	ThrowIfFailed(LoadFromEXRFile(wFilename.c_str(), NULL, scratchImage));
//
//	width = static_cast<int>(metadata.width);
//	height = static_cast<int>(metadata.height);
//	pixelFormat = metadata.format;
//
//	std::cout << filename << " " << metadata.width << " " << metadata.height
//			  << metadata.format << std::endl;
//
//	image.resize(scratchImage.GetPixelsSize());
//	memcpy(image.data(), scratchImage.GetPixels(), image.size());
//
//	// ������ ���� Ȯ���غ���
//	std::vector<float> f32(image.size() / 2);
//	uint16_t*		   f16 = (uint16_t*)image.data();
//	for (int i = 0; i < image.size() / 2; i++)
//	{
//		f32[i] = fp16_ieee_to_fp32_value(f16[i]);
//	}
//
//	const float minValue = *std::min_element(f32.begin(), f32.end());
//	const float maxValue = *std::max_element(f32.begin(), f32.end());
//
//	std::cout << minValue << " " << maxValue << std::endl;
//
//	// f16 = (uint16_t *)image.data();
//	// for (int i = 0; i < image.size() / 2; i++) {
//	//     f16[i] = fp16_ieee_from_fp32_value(f32[i] * 2.0f);
//	// }
// }

void ReadImage(const std::string filename, std::vector<uint8_t>& image,
	int& width, int& height)
{

	int channels;

	unsigned char* img =
		stbi_load(filename.c_str(), &width, &height, &channels, 0);

	// assert(channels == 4);

	std::cout << "ReadImage() " << filename << " " << width << " " << height << " "
			  << channels << std::endl;

	// 4ä�η� ���� ����
	image.resize(width * height * 4);

	if (channels == 1)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			uint8_t g = img[i * channels + 0];
			for (size_t c = 0; c < 4; c++)
			{
				image[4 * i + c] = g;
			}
		}
	}
	else if (channels == 2)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 2; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 2] = 255;
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 3)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 3; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
			image[4 * i + 3] = 255;
		}
	}
	else if (channels == 4)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			for (size_t c = 0; c < 4; c++)
			{
				image[4 * i + c] = img[i * channels + c];
			}
		}
	}
	else
	{
		std::cout << "Cannot read " << channels << " channels" << std::endl;
	}

	delete[] img;
}

void ReadImage(const std::string albedoFilename,
	const std::string opacityFilename, std::vector<uint8_t>& image,
	int& width, int& height)
{

	ReadImage(albedoFilename, image, width, height);

	std::vector<uint8_t> opacityImage;
	int					 opaWidth, opaHeight;

	ReadImage(opacityFilename, opacityImage, opaWidth, opaHeight);

	assert(width == opaWidth && height == opaHeight);

	for (int j = 0; j < height; j++)
		for (int i = 0; i < width; i++)
		{
			image[3 + 4 * i + 4 * width * j] =
				opacityImage[4 * i + 4 * width * j]; // Copy alpha channel
		}
}

Microsoft::WRL::ComPtr<ID3D11Texture2D> D3D11Utils::CreateStagingTexture(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
	const int width, const int height, const std::vector<uint8_t>& image,
	const DXGI_FORMAT pixelFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
	const int mipLevels = 1, const int arraySize = 1)
{

	// ������¡ �ؽ��� �����
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = mipLevels;
	txtDesc.ArraySize = arraySize;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&txtDesc, NULL,
			stagingTexture.GetAddressOf())))
	{
		std::cout << "Failed()" << std::endl;
	}

	size_t pixelSize = GetPixelSize(pixelFormat);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
	uint8_t* pData = (uint8_t*)ms.pData;
	for (UINT h = 0; h < UINT(height); h++)
	{ // ������ �� �پ� ����
		memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize],
			width * pixelSize);
	}
	context->Unmap(stagingTexture.Get(), NULL);

	return stagingTexture;
}

Microsoft::WRL::ComPtr<ID3D11Texture3D> D3D11Utils::CreateStagingTexture3D(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width, const int height,
	const int depth, const DXGI_FORMAT pixelFormat)
{

	// ������¡ �ؽ��� �����
	D3D11_TEXTURE3D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.Depth = depth;
	txtDesc.MipLevels = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.Usage = D3D11_USAGE_STAGING;
	txtDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

	Microsoft::WRL::ComPtr<ID3D11Texture3D> stagingTexture;
	if (FAILED(device->CreateTexture3D(&txtDesc, NULL,
			stagingTexture.GetAddressOf())))
	{
		std::cout << "CreateStagingTexture3D() failed." << std::endl;
	}

	return stagingTexture;
}

void CreateTextureHelper(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const int width,
	const int height, const std::vector<uint8_t>& image,
	const DXGI_FORMAT								  pixelFormat,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  texture,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{

	// ������¡ �ؽ��� ����� CPU���� �̹����� �����մϴ�.
	Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture = D3D11Utils::CreateStagingTexture(
		device, context, width, height, image, pixelFormat);

	// ������ ����� �ؽ��� ����
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 0; // �Ӹ� ���� �ִ�
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT; // ������¡ �ؽ���κ��� ���� ����
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // �Ӹ� ���
	txtDesc.CPUAccessFlags = 0;

	// �ʱ� ������ ���� �ؽ��� ���� (���� ������)
	device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

	// ������ ������ MipLevels�� Ȯ���غ��� ���� ���
	// texture->GetDesc(&txtDesc);
	// std::cout << txtDesc.MipLevels << std::endl;

	// ������¡ �ؽ���κ��� ���� �ػ󵵰� ���� �̹��� ����
	context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0,
		stagingTexture.Get(), 0, NULL);

	// ResourceView �����
	device->CreateShaderResourceView(texture.Get(), 0, srv.GetAddressOf());

	// �ػ󵵸� ���簡�� �Ӹ� ����
	context->GenerateMips(srv.Get());

	// HLSL ���̴� �ȿ����� SampleLevel() ���
}

void D3D11Utils::CreateMetallicRoughnessTexture(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
	const std::string metallicFilename, const std::string roughnessFilename,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{

	// GLTF ����� �̹� ������ ����
	if (!metallicFilename.empty() && (metallicFilename == roughnessFilename))
	{
		CreateTexture(device, context, metallicFilename, false, texture, srv);
	}
	else
	{
		// ���� ������ ��� ���� �о �����ݴϴ�.

		// ReadImage()�� Ȱ���ϱ� ���ؼ� �� �̹������� ���� 4ä�η� ��ȯ �� �ٽ�
		// 3ä�η� ��ġ�� ������� ����
		int					 mWidth = 0, mHeight = 0;
		int					 rWidth = 0, rHeight = 0;
		std::vector<uint8_t> mImage;
		std::vector<uint8_t> rImage;

		// (���� ��������) �� �� �ϳ��� ���� ��쵵 �����ϱ� ���� ���� ���ϸ�
		// Ȯ��
		if (!metallicFilename.empty())
		{
			ReadImage(metallicFilename, mImage, mWidth, mHeight);
		}

		if (!roughnessFilename.empty())
		{
			ReadImage(roughnessFilename, rImage, rWidth, rHeight);
		}

		// �� �̹����� �ػ󵵰� ���ٰ� ����
		if (!metallicFilename.empty() && !roughnessFilename.empty())
		{
			assert(mWidth == rWidth);
			assert(mHeight == rHeight);
		}

		std::vector<uint8_t> combinedImage(size_t(mWidth * mHeight) * 4);
		fill(combinedImage.begin(), combinedImage.end(), 0);

		for (size_t i = 0; i < size_t(mWidth * mHeight); i++)
		{
			if (rImage.size())
				combinedImage[4 * i + 1] = rImage[4 * i]; // Green = Roughness
			if (mImage.size())
				combinedImage[4 * i + 2] = mImage[4 * i]; // Blue = Metalness
		}

		CreateTextureHelper(device, context, mWidth, mHeight, combinedImage,
			DXGI_FORMAT_R8G8B8A8_UNORM, texture, srv);
	}
}

void D3D11Utils::CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>&					 context,
	const std::string filename, const bool usSRGB,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  tex,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv)
{

	int					 width = 0, height = 0;
	std::vector<uint8_t> image;
	DXGI_FORMAT			 pixelFormat =
		 usSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

	std::string ext(filename.end() - 3, filename.end());
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	ReadImage(filename, image, width, height);

	CreateTextureHelper(device, context, width, height, image, pixelFormat, tex,
		srv);
}

void D3D11Utils::CreateTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>&					 context,
	const std::string												 albedoFilename,
	const std::string												 opacityFilename,
	const bool														 usSRGB,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&						 texture,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&				 srv)
{

	int					 width = 0, height = 0;
	std::vector<uint8_t> image;
	DXGI_FORMAT			 pixelFormat =
		 usSRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

	ReadImage(albedoFilename, opacityFilename, image, width, height);

	CreateTextureHelper(device, context, width, height, image, pixelFormat,
		texture, srv);
}

void D3D11Utils::CreateUATexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width,
	const int										   height,
	const DXGI_FORMAT								   pixelFormat,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		   texture,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	   rtv,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav)
{

	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.MipLevels = 1;
	txtDesc.ArraySize = 1;
	txtDesc.Format = pixelFormat; // �ַ� FLOAT ���
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;

	if (!CheckResult(device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf())))
		; // return false;
	if (!CheckResult(device->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf())))
		; // return false;
	if (!CheckResult(device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf())))
		; // return false;
	if (!CheckResult(device->CreateUnorderedAccessView(texture.Get(), NULL, uav.GetAddressOf())))
		; // return false;
		  // return true;
}

void D3D11Utils::CreateTexture3D(Microsoft::WRL::ComPtr<ID3D11Device>& device, const int width,
	const int height, const int depth,
	const DXGI_FORMAT								   pixelFormat,
	const std::vector<float>&						   initData,
	Microsoft::WRL::ComPtr<ID3D11Texture3D>&		   texture,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	   rtv,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav)
{

	D3D11_TEXTURE3D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = width;
	txtDesc.Height = height;
	txtDesc.Depth = depth;
	txtDesc.MipLevels = 1;
	txtDesc.Format = pixelFormat;
	txtDesc.Usage = D3D11_USAGE_DEFAULT;
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
	txtDesc.MiscFlags = 0;
	txtDesc.CPUAccessFlags = 0;

	if (initData.size() > 0)
	{
		size_t				   pixelSize = GetPixelSize(pixelFormat);
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initData.data();
		bufferData.SysMemPitch = UINT(width * pixelSize);
		bufferData.SysMemSlicePitch = UINT(width * height * pixelSize);
		if (CheckResult(device->CreateTexture3D(&txtDesc, &bufferData, texture.GetAddressOf())))
			; // return false;
			  // return true;
	}
	else
	{
		if (!CheckResult(device->CreateTexture3D(&txtDesc, NULL, texture.GetAddressOf())))
			; // return false;
	}

	if (!CheckResult(device->CreateRenderTargetView(texture.Get(), NULL, rtv.GetAddressOf())))
		; // return false;
	if (!CheckResult(device->CreateShaderResourceView(texture.Get(), NULL, srv.GetAddressOf())))
		; // return false;
	if (!CheckResult(device->CreateUnorderedAccessView(texture.Get(), NULL, uav.GetAddressOf())))
		; // return false;
		  // return true;
}

void D3D11Utils::CreateStagingBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const UINT															   numElements,
	const UINT															   sizeElement,
	const void*															   initData,
	Microsoft::WRL::ComPtr<ID3D11Buffer>&								   buffer)
{

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = numElements * sizeElement;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	desc.StructureByteStride = sizeElement;

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initData;
		if (!CheckResult(device->CreateBuffer(&desc, &bufferData, buffer.GetAddressOf())))
			; // return false;
	}
	else
	{
		if (!CheckResult(device->CreateBuffer(&desc, NULL, buffer.GetAddressOf())))
			; // return false;
	}
	// return true;
}

void D3D11Utils::CopyFromStagingBuffer(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
	Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer, UINT size,
	void* dest)
{
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(buffer.Get(), NULL, D3D11_MAP_READ, NULL, &ms);
	memcpy(dest, ms.pData, size);
	context->Unmap(buffer.Get(), NULL);
}

void D3D11Utils::CreateStructuredBuffer(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, const UINT numElements,
	const UINT sizeElement, const void* initData, Microsoft::WRL::ComPtr<ID3D11Buffer>& buffer,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&  srv,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& uav)
{

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = numElements * sizeElement;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | // Compute Shader
		D3D11_BIND_SHADER_RESOURCE;						 // Vertex Shader
	bufferDesc.StructureByteStride = sizeElement;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	// ����: Structured�� D3D11_BIND_VERTEX_BUFFER�� ��� �Ұ�

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initData;
		if (!CheckResult(device->CreateBuffer(&bufferDesc, &bufferData, buffer.GetAddressOf())))
			; // return false;
	}
	else
	{
		if (!CheckResult(device->CreateBuffer(&bufferDesc, NULL, buffer.GetAddressOf())))
			; // return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = numElements;
	device->CreateUnorderedAccessView(buffer.Get(), &uavDesc,
		uav.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.NumElements = numElements;
	device->CreateShaderResourceView(buffer.Get(), &srvDesc,
		srv.GetAddressOf());
}

void D3D11Utils::CreateIndirectArgsBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const UINT																	numElements,
	const UINT																	sizeElement,
	const void*																	initData,
	Microsoft::WRL::ComPtr<ID3D11Buffer>&										buffer)
{

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = numElements * sizeElement;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags =
		D3D11_BIND_UNORDERED_ACCESS; // ComputeShader���� ������Ʈ ����
	desc.StructureByteStride = sizeElement;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS; // <- IndirectArgs

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initData;
		if (!CheckResult(device->CreateBuffer(&desc, &bufferData, buffer.GetAddressOf())))
			; // return false;
	}
	else
	{
		if (!CheckResult(device->CreateBuffer(&desc, NULL, buffer.GetAddressOf())))
			; // return false;
	}
}

void D3D11Utils::CreateAppendBuffer(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	const UINT															  numElements,
	const UINT															  sizeElement,
	const void*															  initData,
	Microsoft::WRL::ComPtr<ID3D11Buffer>&								  buffer,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>&					  srv,
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>&					  uav)
{

	// CreateStructuredBuffer()�� ������ �� UAV ���� ��
	// D3D11_BUFFER_UAV_FLAG_APPEND ��� �� ������ �ٸ�

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = numElements * sizeElement;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | // Compute Shader
		D3D11_BIND_SHADER_RESOURCE;						 // Vertex Shader
	bufferDesc.StructureByteStride = sizeElement;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	if (initData)
	{
		D3D11_SUBRESOURCE_DATA bufferData;
		ZeroMemory(&bufferData, sizeof(bufferData));
		bufferData.pSysMem = initData;
		if (!CheckResult(device->CreateBuffer(&bufferDesc, &bufferData, buffer.GetAddressOf())))
			; // return false;
	}
	else
	{
		if (!CheckResult(device->CreateBuffer(&bufferDesc, NULL, buffer.GetAddressOf())))
			; // return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = numElements;
	uavDesc.Buffer.Flags =
		D3D11_BUFFER_UAV_FLAG_APPEND; // <- AppendBuffer�� ���
	device->CreateUnorderedAccessView(buffer.Get(), &uavDesc,
		uav.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.NumElements = numElements;
	device->CreateShaderResourceView(buffer.Get(), &srvDesc,
		srv.GetAddressOf());
}

void D3D11Utils::CreateTextureArray(
	Microsoft::WRL::ComPtr<ID3D11Device>& device, Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
	const std::vector<std::string> filenames, Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureSRV)
{

	using namespace std;

	if (filenames.empty())
		return;

	// ��� �̹����� width�� height�� ���ٰ� �����մϴ�.

	// ���Ϸκ��� �̹��� ���� ���� �о���Դϴ�.
	int								  width = 0, height = 0;
	std::vector<std::vector<uint8_t>> imageArray;
	for (const auto& f : filenames)
	{

		std::cout << f << std::endl;

		std::vector<uint8_t> image;

		ReadImage(f, image, width, height);

		imageArray.push_back(image);
	}

	UINT size = UINT(filenames.size());

	// Texture2DArray�� ����ϴ�. �̶� �����͸� CPU�κ��� �������� �ʽ��ϴ�.
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width = UINT(width);
	txtDesc.Height = UINT(height);
	txtDesc.MipLevels = 0; // �Ӹ� ���� �ִ�
	txtDesc.ArraySize = size;
	txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.SampleDesc.Quality = 0;
	txtDesc.Usage = D3D11_USAGE_DEFAULT; // ������¡ �ؽ���κ��� ���� ����
	txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // �Ӹ� ���

	// �ʱ� ������ ���� �ؽ��縦 ����ϴ�.
	device->CreateTexture2D(&txtDesc, NULL, texture.GetAddressOf());

	// ������ ������� MipLevels�� Ȯ��
	texture->GetDesc(&txtDesc);
	// std::cout << txtDesc.MipLevels << std::endl;

	// StagingTexture�� ���� �ϳ��� �����մϴ�.
	for (size_t i = 0; i < imageArray.size(); i++)
	{

		auto& image = imageArray[i];

		// StagingTexture�� Texture2DArray�� �ƴ϶� Texture2D �Դϴ�.
		Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture = CreateStagingTexture(
			device, context, width, height, image, txtDesc.Format, 1, 1);

		// ������¡ �ؽ��縦 �ؽ��� �迭�� �ش� ��ġ�� �����մϴ�.
		UINT subresourceIndex =
			D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

		context->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0,
			stagingTexture.Get(), 0, NULL);
	}

	device->CreateShaderResourceView(texture.Get(), NULL,
		textureSRV.GetAddressOf());

	context->GenerateMips(textureSRV.Get());
}

// void D3D11Utils::CreateDDSTexture(
//	Microsoft::WRL::ComPtr<ID3D11Device>& device, const wchar_t* filename, bool isCubeMap,
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView)
//{
//
//	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
//
//	UINT miscFlags = 0;
//	if (isCubeMap)
//	{
//		miscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
//	}
//
//	// https://github.com/microsoft/DirectXTK/wiki/DDSTextureLoader
//	if (!CheckResult(CreateDDSTextureFromFileEx(
//			device.Get(), filename, 0, D3D11_USAGE_DEFAULT,
//			D3D11_BIND_SHADER_RESOURCE, 0, miscFlags, DDS_LOADER_FLAGS(false),
//			(ID3D11Resource**)texture.GetAddressOf(),
//			textureResourceView.GetAddressOf(), NULL)))
//		; // return false;
// }

void D3D11Utils::WriteToPngFile(Microsoft::WRL::ComPtr<ID3D11Device>& device,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>&					  context,
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&						  textureToWrite,
	const std::string												  filename)
{

	D3D11_TEXTURE2D_DESC desc;
	textureToWrite->GetDesc(&desc);
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU���� �б� ����
	desc.Usage = D3D11_USAGE_STAGING;			 // GPU���� CPU�� ���� �����͸� �ӽ� ����

	Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
	if (FAILED(device->CreateTexture2D(&desc, NULL,
			stagingTexture.GetAddressOf())))
	{
		std::cout << "Failed()" << std::endl;
	}

	// ����: ��ü ������ ��
	// context->CopyResource(stagingTexture.Get(), pTemp.Get());

	// �Ϻθ� ������ �� ���
	D3D11_BOX box;
	box.left = 0;
	box.right = desc.Width;
	box.top = 0;
	box.bottom = desc.Height;
	box.front = 0;
	box.back = 1;
	context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0,
		textureToWrite.Get(), 0, &box);

	// R8G8B8A8 �̶�� ����
	std::vector<uint8_t> pixels(desc.Width * desc.Height * 4);

	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
		&ms); // D3D11_MAP_READ ����

	// �ؽ��簡 ���� ��쿡��
	// ms.RowPitch�� width * sizeof(uint8_t) * 4���� Ŭ ���� �־
	// for������ ������ �ϳ��� ����
	uint8_t* pData = (uint8_t*)ms.pData;
	for (unsigned int h = 0; h < desc.Height; h++)
	{
		memcpy(&pixels[h * desc.Width * 4], &pData[h * ms.RowPitch],
			desc.Width * sizeof(uint8_t) * 4);
	}

	context->Unmap(stagingTexture.Get(), NULL);

	stbi_write_png(filename.c_str(), desc.Width, desc.Height, 4, pixels.data(),
		desc.Width * 4);

	std::cout << filename << std::endl;
}

size_t D3D11Utils::GetPixelSize(DXGI_FORMAT pixelFormat)
{

	switch (pixelFormat)
	{
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			return sizeof(uint16_t) * 4;
		case DXGI_FORMAT_R32G32B32A32_FLOAT:
			return sizeof(uint32_t) * 4;
		case DXGI_FORMAT_R32_FLOAT:
			return sizeof(uint32_t) * 1;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			return sizeof(uint8_t) * 4;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return sizeof(uint8_t) * 4;
		case DXGI_FORMAT_R32_SINT:
			return sizeof(int32_t) * 1;
		case DXGI_FORMAT_R16_FLOAT:
			return sizeof(uint16_t) * 1;
	}

	std::cout << "PixelFormat not implemented " << pixelFormat << std::endl;

	return sizeof(uint8_t) * 4;
}