#include "ShieldBackSide.h"
#include "imgui.h"

ShieldBackSide::ShieldBackSide()
{
	//instance = this;
	model = std::make_unique<Model>("Data/Model/Shield/shield.mdl");
	scale.x = scale.y = scale.z = 0.0006f;
	angle.z = 1.55f;
	angle.y = 4.75f;
}

ShieldBackSide::~ShieldBackSide()
{
	//delete model;
}

void ShieldBackSide::Update(float elapsedTime)
{
	position.x = targetPosition.x + RangeToPlayer;
	position.z = targetPosition.z + RangeToPlayer;

	deltaRotation = rotationSpeed * elapsedTime;

	rotationAngle += deltaRotation;

	float cosAngle = cos(rotationAngle);
	float sinAngle = sin(rotationAngle);
	float offsetX = position.x - targetPosition.x;
	float offsetZ = position.z - targetPosition.z;
	float rotatedOffsetX = offsetX * cosAngle - offsetZ * sinAngle;
	float rotatedOffsetZ = offsetX * sinAngle + offsetZ * cosAngle;
	float newX = targetPosition.x + rotatedOffsetX;
	float newZ = targetPosition.z + rotatedOffsetZ;

	position.x = newX;
	position.y = 1.1f;
	position.z = newZ;

	// Calculate the direction vector from the position to the target
	float directionX = targetPosition.x - position.x;
	float directionZ = targetPosition.z - position.z;

	// Calculate the rotation angle to face the opposite direction
	angle.y = atan2(directionX, directionZ) + DirectX::XM_PI + 1.6f;

	//position = DirectX::XMFLOAT3(targetPosition.x, targetPosition.y + 1.1f, targetPosition.z - radius);
	UpdateTransform();

	// Update the model's transform with the updated transformation matrix
	model->UpdateTransform(transform);
}

//void ShieldBackSide::Render(ID3D11DeviceContext* dc, Shader* shader)
//{
//	shader->Draw(dc, model);
//}

void ShieldBackSide::DrawDebugUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Shield", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ˆÊ’u
			ImGui::InputFloat3("Position", &position.x);
			ImGui::InputFloat("Rotation", &deltaRotation);
			//‰ñ“]
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			ImGui::InputFloat3("Angle", &a.x);
		}
	}
	ImGui::End();
}

void ShieldBackSide::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}