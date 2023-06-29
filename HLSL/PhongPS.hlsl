#include "Phong.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);	// �@���}�b�v
Texture2D shadowMap : register(t2);	// �V���h�E�}�b�v

SamplerState diffuseMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

	// �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
	float3 normal = normalMap.Sample(diffuseMapSamplerState, pin.texcoord).xyz * 2 - 1;
	float3x3 CM   = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
	float3 N      = normalize(mul(normal, CM));

	float3 L = normalize(directionalLightData.direction.xyz);
	float3 E = normalize(viewPosition.xyz - pin.world_position.xyz);

	// �����̌v�Z
	float3 ambient = ka.rgb * ambientLightColor.rgb * diffuseColor.rgb;

	// ���s�����̃��C�e�B���O�v�Z
	float3 diffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, kd.rgb);
	float3 specular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks.rgb);

	// ���s�����̉e�Ȃ̂ŁA���s�����ɑ΂��ĉe��K��
	float3 shadow = CalcShadowColor(shadowMap, shadowMapSamplerState, pin.shadowTexcoord, shadowColor, shadowBias);
	diffuse *= shadow;
	specular *= shadow;

	// �_�����̏���
	int i; 
	for (i = 0; i < pointLightCount; ++i)
	{
		// ���C�g�x�N�g�����Z�o
		float3 lightVector = pin.world_position - pointLightData[i].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
		float lightLength = length(lightVector);

		// ���C�g�̉e���͈͊O�Ȃ��̌v�Z�����Ȃ��B
		if (lightLength >= pointLightData[i].range)
			continue;

		// �����������Z�o����
		float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

		lightVector = lightVector / lightLength;
		diffuse += CalcLambertDiffuse(N, lightVector,
									pointLightData[i].color.rgb, kd.rgb) * attenuate;
		specular += CalcPhongSpecular(N, lightVector,
									pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
	}

	// �X�|�b�g���C�g�̏���
	for (i = 0; i < spotLightCount; ++i)
	{
		// ���C�g�x�N�g�����Z�o
		float3 lightVector = pin.world_position - spotLightData[i].position.xyz;

		// ���C�g�x�N�g���̒������Z�o
		float lightLength = length(lightVector);

		if (lightLength >= spotLightData[i].range)
			continue;

		// �����������Z�o����
		float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

		lightVector = normalize(lightVector);

		// �p�x�������Z�o����attenuate�ɏ�Z����
		float3 spotDirection = normalize(spotLightData[i].direction.xyz);
		float angle          = dot(spotDirection, lightVector);
		float area           = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
		attenuate           *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

		diffuse += CalcLambertDiffuse(N, lightVector,
									spotLightData[i].color.rgb, kd.rgb) * attenuate;
		specular += CalcPhongSpecular(N, lightVector,
									spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
	}

	return float4(diffuseColor.rgb * (ambient + diffuse) + specular, diffuseColor.a);
}
