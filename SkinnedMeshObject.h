#pragma once

#include "Object.h"

class SkinnedMeshObject
{
public:
	SkinnedMeshObject(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&		context,
		const std::vector<MeshData>&						meshes,
		const AnimationData&								aniData)
	{
		Initialize(device, context, meshes, aniData);
	}

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	  context,
		const std::vector<MeshData>&					  meshes,
		const AnimationData&							  aniData)
	{
		InitAnimationData(device, aniData);
		Object::Initialize(device, context, meshes);
	}

	void InitMeshBuffers(Microsoft::WRL::ComPtr<ID3D11Device>& device, const MeshData& meshData,
		shared_ptr<Mesh>& newMesh) override
	{
		D3D11Utils::CreateVertexBuffer(device, meshData.skinnedVertices,
			newMesh->vertexBuffer);
		newMesh->indexCount = UINT(meshData.indices.size());
		newMesh->vertexCount = UINT(meshData.skinnedVertices.size());
		newMesh->stride = UINT(sizeof(SkinnedVertex));
		D3D11Utils::CreateIndexBuffer(device, meshData.indices,
			newMesh->indexBuffer);
	}

	void InitAnimationData(Microsoft::WRL::ComPtr<ID3D11Device>& device,
		const AnimationData&									 aniData)
	{
		if (!aniData.clips.empty())
		{
			m_aniData = aniData;

			// ���⼭�� AnimationClip�� SkinnedMesh��� �����ϰڽ��ϴ�.
			// �Ϲ������δ� ��� Animation�� SkinnedMesh Animation�� �ƴմϴ�.
			m_boneTransforms.m_cpu.resize(aniData.clips.front().keys.size());

			// ����: ��� keys() ������ �������� ���� ���� �ֽ��ϴ�.
			for (int i = 0; i < aniData.clips.front().keys.size(); i++)
				m_boneTransforms.m_cpu[i] = Matrix();
			m_boneTransforms.Initialize(device);
		}
	}

	void UpdateAnimation(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, int clipId,
		int frame) override
	{

		m_aniData.Update(clipId, frame);

		for (int i = 0; i < m_boneTransforms.m_cpu.size(); i++)
		{
			m_boneTransforms.m_cpu[i] =
				m_aniData.Get(clipId, i, frame).Transpose();
		}

		m_boneTransforms.Upload(context);
	}

	void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context) override
	{

		// ConstBuffer ��� StructuredBuffer ���
		// context->VSSetConstantBuffers(3, 1, m_skinnedConsts.GetAddressOf());

		context->VSSetShaderResources(
			9, 1, m_boneTransforms.GetAddressOfSRV()); // �׻� slot index ����

		// Skinned VS/PS�� GetPSO()�� ���ؼ� �����Ǳ� ������
		// Model::Render(.)�� ���� ��� ����

		Model::Render(context);
	};

	// SkinnedMesh�� BoundingBox�� �׸� �� Root�� Transform�� �ݿ��ؾ� �մϴ�.
	// virtual void RenderWireBoundingBox(Microsoft::WRL::ComPtr<ID3D11DeviceContext> &context);
	// virtual void RenderWireBoundingSphere(Microsoft::WRL::ComPtr<ID3D11DeviceContext>
	// &context);

public:
	StructuredBuffer<Matrix> m_boneTransforms;

	AnimationData m_aniData;
};
