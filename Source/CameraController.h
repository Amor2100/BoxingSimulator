#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	CameraController();
	~CameraController() {}

	// 更新処理
	void Update(float elapsedTime);

	void SetTarget(const DirectX::XMFLOAT3& target) { this->camera_focus = target; }
	void SetEnemy(const DirectX::XMFLOAT3& target) { this->camera_enemy = target; }

	void DrawDebugUI();

	bool GetStats(bool stats) { return this->ready = stats; }
	bool GetHit(bool hit) { return this->isTargetHit = hit; }

private:
	float					rotateX = DirectX::XMConvertToRadians(45);
	float					rotateY = 0.0f;
	float					distance = 10.0f;
	DirectX::XMFLOAT3 camera_position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 camera_focus{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 camera_enemy{ 0.0f, 0.0f, 0.0f };

	DirectX::XMFLOAT3	target = { 0, 0, 0 };
	DirectX::XMFLOAT3	angle = { 0, 0, 0 };
	float				rollSpeed = DirectX::XMConvertToRadians(90);
	float				range = 2.0f;
	float				maxAngleX = DirectX::XMConvertToRadians(45);
	float				minAngleX = DirectX::XMConvertToRadians(-45);
	bool ready = false;

	// Camera Shake
	bool isTargetHit = false;
	float cameraShakeMagnitude = 0.1f;
	float cameraShakeTimer = 0.8f;
	DirectX::XMFLOAT3 originalCameraPosition = { 0,0,0 };
};
