#include "StageMain.h"

StageMain::StageMain()
{
	//instance = this;
	model = std::make_unique<Model>("Data/Model/ExampleStage/examplestage.mdl");
}

StageMain::~StageMain()
{
	//delete model;
}

void StageMain::Update(float elapsedTime)
{
	UpdateTransform();
	model->UpdateTransform(transform);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), hit);
}

void StageMain::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}