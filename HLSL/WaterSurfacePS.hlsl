#include "WaterSurface.hlsli"
#include "CommonFunctions.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1); // �@���}�b�v
Texture2D skyboxMap : register(t2); // ��e�N�X�`��
Texture2D shadowMap : register(t3); // �V���h�E�}�b�v


SamplerState diffuseMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �g�̗h�炬��UV���W�ɗ^����
    float2 texcoord1 = pin.texcoord + wave1;
    float2 texcoord2 = pin.texcoord + wave2;

    
    float4 diffuseColor;
	{
        float4 DC1 = diffuseMap.Sample(diffuseMapSamplerState, texcoord1);
        float4 DC2 = diffuseMap.Sample(diffuseMapSamplerState, texcoord2);
        diffuseColor = ((DC1 + DC2) * 0.5f) * pin.color;
    }

    float3 N;
	{
        float3 N1 = normalMap.Sample(diffuseMapSamplerState, texcoord1).xyz * 2 - 1;
        float3 N2 = normalMap.Sample(diffuseMapSamplerState, texcoord2).xyz * 2 - 1;
        N = N1 + N2;
        N = normalize(normalize(pin.tangent) * N.x +
					normalize(pin.binormal) * N.y +
					normalize(pin.normal) * N.z);
    }

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
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

        diffuse += CalcLambertDiffuse(N, lightVector,
									spotLightData[i].color.rgb, kd.rgb) * attenuate;
        specular += CalcPhongSpecular(N, lightVector,
									spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }

    float4 color = float4(diffuseColor.rgb * (ambient + diffuse) + specular, diffuseColor.a);

	// ���ːF
    float3 reflectColor = 0.0f;
	{
		// ������@���Ŕ��˂����ʒu�̐F���擾����
        //float3 R = reflect(-E, float3(0, 1, 0));
        float3 R = reflect(-E, N);

		// �����~���}�@�ɑ����č쐬���ꂽ�p�m���}�摜����T���v�����O���邽�߂�UV���W���Z�o����
        float2 texcoord = EquirectangularProjection(R);
        reflectColor = skyboxMap.Sample(diffuseMapSamplerState, texcoord).rgb;
    }
    //return float4(color.rgb + reflectColor, color.a);
    
    // shlick�̋ߎ����𗘗p���Ĕ��˗����Z�o���āA�����x�Ƌ�e�N�X�`���Ƃ̃u�����h����ς���
    float n1 = 0.1002f;
    float n2 = 0.1333f;
    float f0 = ((n1 - n2) * (n1 - n2)) / ((n1 + n2) * (n1 + n2));
    
    // �t���l�����˗��̌v�Z
    float reflectance = f0 + (1 - f0) * pow(1 - cos(dot(E, N)), 5);
    
    // �J���[�Ƌ�e�N�X�`���Ƃ��u�����h����
    color.rgb += reflectColor * reflectance;
    
	// ����͐��[���l���Ȃ����ߒP���ɓ����x�ɓK�����Ă��܂��Ƃقڐ��ꂪ�����Ă��܂��B
	// �Ȃ̂Ńe�L�g�[�ɌW�������Z���Ă���
    color.a *= saturate(reflectance + 0.4f);
    
    return color;

}
