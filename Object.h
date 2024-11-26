#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl.h>

#include "AssetLoader.h"
#include "ShaderData.h"

class Object
{
public:
	Object(std::string path, std::string name);

	const size_t						  GetVSize() const;
	const size_t						  GetISize() const;
	const VERTEX_TYPE const*			  GetVData() const;
	const INDEX_TYPE const*				  GetIData() const;
	VertexConstantData*					  GetPVertexConstantData();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& GetIndexBuffer();

	void Transform(DirectX::SimpleMath::Vector3 vec);
	void RotateX(float rad);

private:
	std::vector<VERTEX_TYPE> mVertices;
	std::vector<INDEX_TYPE>	 mIndices;
	VertexConstantData		 mVertexConstantData;

	DirectX::SimpleMath::Vector3 mPosWorld = { 0.0f, 30.0f, 60.0f };

	Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mIndexBuffer;
};