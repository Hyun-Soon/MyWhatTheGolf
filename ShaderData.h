#pragma once

#include <directxtk/SimpleMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex
{
	DirectX::SimpleMath::Vector3 point;
	DirectX::SimpleMath::Vector2 uv;

	Vertex(aiVector3D& aiV)
		: point(aiV.x, aiV.y, aiV.z), uv(0.0f, 0.0f) {}
};

struct VertexConstantData
{
	DirectX::SimpleMath::Matrix model;
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
	DirectX::SimpleMath::Matrix invTranspose;
};
static_assert((sizeof(VertexConstantData) % 16) == 0, "Vertex Constant Buffer size must be 16-byte aligned.");