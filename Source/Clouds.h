#pragma once
#include "Graphics/Model.h"
#include "character.h"
#include "Effect.h"
#include "Graphics/Shader.h"

class Clouds : public Character
{
public:
	Clouds();
	~Clouds();

	void Update(float elapsedTime);

	void DrawDebugPrimitive();
	void DrawDebugUI();
	void UpdateTransform();
	std::unique_ptr<Model> model = nullptr;

private:

	//Model* model = nullptr;
	float rotationAngle = 0.0f;
	float deltaRotation = 0.0f;
	float maxRotationSpeed = 0.4f;
	float rotationSpeed = 1.1f;
	float turnSpeed = DirectX::XMConvertToRadians(360);
	float moveSpeed = 2.20;
	float radius = 0.4f;
};