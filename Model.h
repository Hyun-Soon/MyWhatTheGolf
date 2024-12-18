#pragma once

#include <fstream>

#include "ConstantBuffer.h"
#include "D3D11Utils.h"
#include "ShaderData.h"
#include "Mesh.h"
#include "AssetLoader.h"

class Model
{
public:
	__declspec(align(16)) struct ModelConstants
	{
		DirectX::SimpleMath::Matrix world = DirectX::SimpleMath::Matrix();
	};

	Model(Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
		const std::string&							 basePath,
		const std::string&							 filename);

	virtual ~Model();

	virtual void InitMeshBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const MeshData&												   meshData,
		Mesh*														   newMesh);

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	  context,
		const std::string& basePath, const std::string& filename);

private:
	DirectX::SimpleMath::Matrix m_worldRow = DirectX::SimpleMath::Matrix(); // Model(Model) To World За·Д

	std::vector<Mesh*> mMeshes;

	ConstantBuffer<ModelConstants> mModelConsts; // TODO :: setting and upload
};
