#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl.h>

#include "AssetLoader.h"
#include "ShaderData.h"

class Object
{
public:
	Object(std::string path, std::string filename);

	const size_t									  GetVSize() const;
	const size_t									  GetISize() const;
	const VERTEX_TYPE const*						  GetVData() const;
	const INDEX_TYPE const*							  GetIData() const;
	const char*										  GetCstyleTextureFilename() const;
	VertexConstantData*								  GetPVertexConstantData();
	Microsoft::WRL::ComPtr<ID3D11Buffer>&			  GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>&			  GetIndexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  GetTexture();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetTextureResourceView();

	void Transform(DirectX::SimpleMath::Vector3 vec);
	void RotateX(float rad);
	void RotateY(float rad);

private:
	std::vector<MeshData> mMeshes;
	AnimationData		  mAniData;
	VertexConstantData	  mVertexConstantData;

	DirectX::SimpleMath::Vector3 mPosWorld = { 0.0f, 0.0f, 5.0f };

	Microsoft::WRL::ComPtr<ID3D11Buffer>			 mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			 mIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>			 mTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureResourceView;
};