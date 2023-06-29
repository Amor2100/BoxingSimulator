#include "FilterFunctions.hlsli"
#include "Light.hlsli"

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer CBFinalpass : register(b0)
{
	float4					screenSize;					// xy : �T�C�Y, z : near�N���b�v, w : far�N���b�v
	//	�J�������
	float4					viewPosition;
	row_major	float4x4	view;
	row_major	float4x4	projection;
	row_major	float4x4	inverseView;
	row_major	float4x4	inverseProjection;
	//	�F���␳
	float					hueShift;					// �F������
	float					saturation;					// �ʓx����
	float					brightness;					// ���x����
	float					colorGradiationDummy;
};

