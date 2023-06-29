#pragma once
#include "Graphics/Model.h"
#include "character.h"
#include "Effect.h"
#include "Graphics/Shader.h"

class Shield : public Character
{
public:
	Shield();
	~Shield();

	void Update(float elapsedTime);
	//void Render(ID3D11DeviceContext* dc, Shader* shader);

	void DrawDebugPrimitive();
	void DrawDebugUI();
	void UpdateTransform();
	std::unique_ptr<Model> model = nullptr;

private:

	float rotationAngle = 0.0f;
	float deltaRotation = 0.0f;
	float maxRotationSpeed = 0.4f;
	float rotationSpeed = 1.1f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float moveSpeed = 2.20;
	float radius = 0.4f;
	float RangeToPlayer = 0.35f;
};