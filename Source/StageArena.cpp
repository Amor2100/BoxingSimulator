#include "StageArena.h"

StageArena::StageArena()
{
	//instance = this;
	model = std::make_unique<Model>("Data/Model/Arena/circlearena.mdl");
	scale.x = scale.y = scale.z = 0.004f;
}

StageArena::~StageArena()
{
	//delete model;
}

void StageArena::Update(float elapsedTime)
{
	UpdateTransform();
	model->UpdateTransform(transform);
}

bool StageArena::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return 0;
}

void StageArena::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}