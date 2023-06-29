#pragma once
//#include "Graphics/Shader.h"
#include "Graphics/Model.h"
//#include "Collision.h"
#include "Stage.h"

class StageArena : public Stage
{
public:
	StageArena();
	~StageArena() override;

	void Update(float elapsedTime) override;

	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

	void UpdateTransform();
	std::unique_ptr<Model> model = nullptr;

private:
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
};