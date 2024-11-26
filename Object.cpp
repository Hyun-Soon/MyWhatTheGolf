#include "Object.h"

Object::Object(const std::string path, const std::string name)
{
	AssetLoader::Load(path, name, this->mVertices, this->mIndices);

	// temp
	mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld); // DirectX::SimpleMath::Matrix::CreateRotationY(DirectX::XMConvertToRadians(120.0f)) *
	mVertexConstantData.view = DirectX::XMMatrixLookToLH(
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f),
		DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));
	mVertexConstantData.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), float(1280) / 960, 1.0f, 10000.0f); // temp farZ

	mVertexConstantData.model = mVertexConstantData.model.Transpose();
	mVertexConstantData.view = mVertexConstantData.view.Transpose();
	mVertexConstantData.projection = mVertexConstantData.projection.Transpose();
}

const size_t Object::GetVSize() const
{
	return this->mVertices.size();
}

const size_t Object::GetISize() const
{
	return this->mIndices.size();
}

const Vertex const* Object::GetVData() const
{
	return this->mVertices.data();
}

const uint32_t const* Object::GetIData() const
{
	return this->mIndices.data();
}

VertexConstantData* Object::GetPVertexConstantData()
{
	return &(this->mVertexConstantData);
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Object::GetVertexBuffer()
{
	return this->mVertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& Object::GetIndexBuffer()
{
	return this->mIndexBuffer;
}
void Object::Transform(DirectX::SimpleMath::Vector3 vec)
{
	mPosWorld += vec;
	this->mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld);
	this->mVertexConstantData.model = this->mVertexConstantData.model.Transpose();
}
void Object::RotateX(float rad)
{
	this->mVertexConstantData.model = DirectX::SimpleMath::Matrix::CreateRotationX(rad) * DirectX::SimpleMath::Matrix::CreateTranslation(mPosWorld);
	this->mVertexConstantData.model = this->mVertexConstantData.model.Transpose();
}
// device, context -> must handled by directXManager