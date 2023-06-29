#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "SceneGame.h"

CameraController::CameraController()
{
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(0, 75, 75),
		DirectX::XMFLOAT3(0, 0, 0),
		DirectX::XMFLOAT3(0, 1, 0)
	);
	camera.SetPerspectiveFov(
		DirectX::XMConvertToRadians(45),
		1280.0f / 720.0f,
		1.0f,
		10000.0f
	);
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&camera.GetEye());
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera.GetFocus());
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Eye, Focus);
	DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);
	DirectX::XMStoreFloat(&distance, Length);

	DirectX::XMFLOAT3 vec;
	DirectX::XMStoreFloat3(&vec, DirectX::XMVectorDivide(Vec, Length));

	float f = sqrtf(vec.x * vec.x + vec.z * vec.z);

	rotateX = atan2f(vec.y, f);
	rotateY = atan2f(vec.x, vec.z);
}

// XVˆ—
void CameraController::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();
	Graphics& graphics = Graphics::Instance();
	
	float moveX = (mouse.GetPositionX() - mouse.GetScreenWidth() / 2) * 0.01f;
	float moveY = (mouse.GetPositionY() - mouse.GetScreenHeight() / 2) * 0.01f;
	
	if (ready)
	{
		SetCursorPos(graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2);
	}
	
	// Y-axis rotation
	rotateY += moveX * 0.2f;
	if (rotateY > DirectX::XM_PI)
	rotateY -= DirectX::XM_2PI;
	else if (rotateY < -DirectX::XM_PI)
		rotateY += DirectX::XM_2PI;
	
	// X-axis rotation
	rotateX += moveY * 0.2f;
	if (rotateX > DirectX::XMConvertToRadians(45.0f))
	rotateX = DirectX::XMConvertToRadians(45.0f);
	else if (rotateX < -DirectX::XMConvertToRadians(45.0f))
		rotateX = -DirectX::XMConvertToRadians(45.0f);
	
	float sx = ::sinf(rotateX), cx = ::cosf(rotateX);
	float sy = ::sinf(rotateY), cy = ::cosf(rotateY);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&camera_focus);
	DirectX::XMVECTOR Front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
	DirectX::XMVECTOR Distance = DirectX::XMVectorSet(2.0f, 1.0f, 2.0f, 0.0f);
	Front = DirectX::XMVectorMultiply(Front, Distance);
	DirectX::XMVECTOR Eye = DirectX::XMVectorSubtract(Focus, Front);
	DirectX::XMStoreFloat3(&camera_position, Eye);

	// Opposite Direction of Enemy
	DirectX::XMFLOAT3 oppositeDirection;
	oppositeDirection.x = camera_focus.x - camera_enemy.x;
	oppositeDirection.y = camera_focus.y - camera_enemy.y;
	oppositeDirection.z = camera_focus.z - camera_enemy.z;

	// Normalize the opposite direction vector
	float oppositeDirectionLength = sqrt(oppositeDirection.x * oppositeDirection.x +
		oppositeDirection.y * oppositeDirection.y +
		oppositeDirection.z * oppositeDirection.z);
	oppositeDirection.x /= oppositeDirectionLength;
	oppositeDirection.y /= oppositeDirectionLength;
	oppositeDirection.z /= oppositeDirectionLength;

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		camera_position.x = camera_focus.x + oppositeDirection.x * 2.0f;
		camera_position.y = camera_focus.y + oppositeDirection.y * 1.0f;
		camera_position.z = camera_focus.z + oppositeDirection.z * 2.0f;
	}

	originalCameraPosition = camera_position;
	if (isTargetHit)
	{
		// Convert camera_position to a DirectX::XMVECTOR
		DirectX::XMVECTOR cameraPositionVector = DirectX::XMLoadFloat3(&camera_position);

		// Apply the shake offset to the camera's position along the up vector
		float shakeOffset = sinf(cameraShakeTimer * 80.0f) * cameraShakeMagnitude;
		DirectX::XMVECTOR upVector = { 0, 1, 0 };
		DirectX::XMVECTOR shakeOffsetVector = DirectX::XMVectorScale(upVector, shakeOffset);

		// Add the shake offset to the camera's position
		cameraPositionVector = DirectX::XMVectorAdd(cameraPositionVector, shakeOffsetVector);

		// Store the updated camera position back into camera_position
		DirectX::XMStoreFloat3(&camera_position, cameraPositionVector);

		// Reduce the shake timer
		cameraShakeTimer -= elapsedTime * 3;

		// Reset the shake effect if the timer has expired
		if (cameraShakeTimer <= 0.0f)
		{
			isTargetHit = false;
			cameraShakeTimer = 0.0f;
			camera_position.x = originalCameraPosition.x; // Reset camera position
		}
	}
	else
	{
		cameraShakeTimer = 0.8f;
	}
	
	Camera::Instance().SetLookAt(camera_position, camera_focus, { 0, 1, 0 });

}


void CameraController::DrawDebugUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("CameraController", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::InputFloat3("Pos", &camera_position.x);
			ImGui::InputFloat3("Focus", &camera_focus.x);
			ImGui::InputFloat("cameraShakeMagnitude", &cameraShakeMagnitude);
			ImGui::InputFloat("cameraShakeTimer", &cameraShakeTimer);
		}
	}
	ImGui::End();
}