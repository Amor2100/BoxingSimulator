#include "Clouds.h"
#include "imgui.h"

Clouds::Clouds()
{
	//instance = this;
	model = std::make_unique<Model>("Data/Model/Clouds/clouds.mdl");
	scale.x = scale.z = 0.000012f;
	scale.y = 0.00003f;
}

Clouds::~Clouds()
{
	//delete model;
}

void Clouds::Update(float elapsedTime)
{
	deltaRotation = rotationSpeed * elapsedTime;

	angle.y += deltaRotation;
	position = DirectX::XMFLOAT3(targetPosition.x, targetPosition.y + 1.5f, targetPosition.z);
	UpdateTransform();

	// Update the model's transform with the updated transformation matrix
	model->UpdateTransform(transform);
}

void Clouds::DrawDebugUI()
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

void Clouds::UpdateTransform()
{
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}