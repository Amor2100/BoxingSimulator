#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

//	���s�������
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
};

// �_�������
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	color;		// �F
	float				range;		// �͈�
	DirectX::XMFLOAT3	dummy;
};

// �_�����̍ő吔
static	constexpr	int	PointLightMax = 8;

// �X�|�b�g���C�g���
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// ���W
	DirectX::XMFLOAT4	direction;	// ����
	DirectX::XMFLOAT4	color;		// �F
	float				range;		// �͈�
	float				innerCorn; 	// �C���i�[�p�x�͈�
	float				outerCorn; 	// �A�E�^�[�p�x�͈�
	float				dummy;
};

// �X�|�b�g���C�g�̍ő吔
static	constexpr	int	SpotLightMax = 8;

// �F���␳���
struct ColorGradingData
{
	float	hueShift	= 0;	// �F������
	float	saturation	= 1;	// �ʓx����
	float	brightness	= 1;	// ���x����
	float	dummy;
};

// �K�E�X�t�B���^�[�v�Z���
struct GaussianFilterData
{
	int					kernelSize	= 15;		// �J�[�l���T�C�Y
	float				deviation	= 10.0f;	// �W���΍�
	DirectX::XMFLOAT2	textureSize;			// �򂷃e�N�X�`���̃T�C�Y
};

// �K�E�X�t�B���^�[�̍ő�J�[�l���T�C�Y
static const int MaxKernelSize = 16;

// ���P�x���o�p���
struct LuminanceExtractionData
{
	float				threshold	= 0.514f;	// 臒l
	float				intensity	= 5.015f;	// �u���[���̋��x
	DirectX::XMFLOAT2	dummy2;
};

// �|�X�g�G�t�F�N�g�̍ŏI�p�X�p���
struct FinalpassnData
{
	//	�u���[���e�N�X�`��
	ID3D11ShaderResourceView* bloomTexture;
	//	�J���[�e�N�X�`��
	ID3D11ShaderResourceView* colorMap;
};

//	�V���h�E�}�b�v�p���
struct ShadowMapData
{
	ID3D11ShaderResourceView*	shadowMap;				//	�V���h�E�}�b�v�e�N�X�`��
	DirectX::XMFLOAT4X4			lightViewProjection;	//	���C�g�r���[�v���W�F�N�V�����s��
	DirectX::XMFLOAT3			shadowColor;			//	�e�̐F
	float						shadowBias;				//	�[�x��r�p�̃I�t�Z�b�g�l
};

//	�X�J�C�{�b�N�X�p���
struct SkyboxData
{
	ID3D11ShaderResourceView* skybox;	//	��e�N�X�`��
};


// �����_�[�R���e�L�X�g
struct RenderContext
{
	ID3D11DeviceContext*		deviceContext;
	float timer;
	DirectX::XMFLOAT4			screenSize;	//	xy : �X�N���[���̃T�C�Y, z : near�N���b�v�����Aw : far�N���b�v����

	//	�J�������
	DirectX::XMFLOAT4			viewPosition;
	DirectX::XMFLOAT4X4			view;
	DirectX::XMFLOAT4X4			projection;

	//	���C�g���
	DirectX::XMFLOAT4			ambientLightColor;				// �������
	DirectionalLightData		directionalLightData;			// ���s�������
	PointLightData				pointLightData[PointLightMax];	// �_�������
	SpotLightData				spotLightData[SpotLightMax];	// �X�|�b�g���C�g���
	int							pointLightCount = 0;			// �_������
	int							spotLightCount = 0;				// �X�|�b�g���C�g��

	//	�F���␳���
	ColorGradingData			colorGradingData;

	//	�K�E�X�t�B���^�[���
	GaussianFilterData			gaussianFilterData;

	//	���P�x���o�p���
	LuminanceExtractionData		luminanceExtractionData;

	//	�ŏI�p�X���
	FinalpassnData				finalpassnData;

	//	�V���h�E�}�b�v���
	ShadowMapData				shadowMapData;

	//	�X�J�C�{�b�N�X���
	SkyboxData					skyboxData;

};
