#pragma once

#include <vector>

// debug
#include <iostream>

#include "ShaderData.h"

class AssetLoader
{
public:
	static bool Load(const std::string& basePath, const std::string& filename, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
	{
		// Import Asset
		Assimp::Importer importer;
		const aiScene*	 scene = importer.ReadFile(basePath + filename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
			return false;
		}

		// std::vector<DirectX::XMFLOAT3> normals;
		// std::vector<DirectX::XMFLOAT2> uvs;

		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];
			for (unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				aiVector3D pos = mesh->mVertices[j];
				vertices.emplace_back(pos);

				// if (mesh->HasNormals())
				//{
				//	aiVector3D normal = mesh->mNormals[j];
				//	normals.push_back(DirectX::XMFLOAT3(normal.x, normal.y, normal.z));
				// }

				// if (mesh->mTextureCoords[0])
				//{
				//	aiVector3D texCoord = mesh->mTextureCoords[0][j];
				//	uvs.push_back(DirectX::XMFLOAT2(texCoord.x, texCoord.y));
				// }
			}

			for (unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];
				for (unsigned int k = 0; k < face.mNumIndices; k++)
				{
					indices.emplace_back(face.mIndices[k]);
				}
			}
		}
		return true;
	}
};
