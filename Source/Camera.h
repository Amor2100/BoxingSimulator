#pragma once

#include <DirectXMath.h>

// �J����
class Camera
{
private:
	Camera() {}
	~Camera() {}

public:
	// �B��̃C���X�^���X�擾
	static Camera& Instance()
	{
		static Camera camera;
		return camera;
	}

	// �w�����������
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// �p�[�X�y�N�e�B�u�ݒ�
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// �r���[�s��擾
	const DirectX::XMFLOAT4X4& GetView() const { return view; }

	// �v���W�F�N�V�����s��擾
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// ���_�擾
	const DirectX::XMFLOAT3& GetEye() const { return eye; }

	// �����_�擾
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }

	// ������擾
	const DirectX::XMFLOAT3& GetUp() const { return up; }

	// �O�����擾
	const DirectX::XMFLOAT3& GetFront() const { return front; }

	// �E�����擾
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// �߃N���b�v�����擾
	const float& GetNearZ() const { return nearZ; }

	// ���N���b�v�����擾
	const float& GetFarZ() const { return farZ; }

private:
	DirectX::XMFLOAT4X4		view;
	DirectX::XMFLOAT4X4		projection;

	DirectX::XMFLOAT3		eye{0, 0, 10};
	DirectX::XMFLOAT3		focus{0, 0, 0};

	DirectX::XMFLOAT3		up{0, 1, 0};
	DirectX::XMFLOAT3		front{0, 0, 1};
	DirectX::XMFLOAT3		right{1, 0, 0};

	float					nearZ;
	float					farZ;
};