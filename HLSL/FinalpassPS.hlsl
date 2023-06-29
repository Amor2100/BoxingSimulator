#include "Finalpass.hlsli"

// �V�[���e�N�X�`��
Texture2D sceneTexture : register(t0);

// �u���[���e�N�X�`��
Texture2D bloomTexture : register(t1);

// �V�[���T���v���X�e�[�g
SamplerState sceneSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �J���[�̎擾
	float4 color = sceneTexture.Sample(sceneSampler, pin.texcoord) * pin.color;

	// �u���[���e�N�X�`�������Z����
	color.rgb += bloomTexture.Sample(sceneSampler, pin.texcoord).rgb;

	// �F���␳����
	{
		// RGB > HSV�ɕϊ�
		color.rgb = RGB2HSV(color.rgb);

		// �F������
		color.r += hueShift;

		// �ʓx����
		color.g *= saturation;

		// ���x����
		color.b *= brightness;

		// HSV > RGB�ɕϊ�
		color.rgb = HSV2RGB(color.rgb);
	}

	return color;
}

