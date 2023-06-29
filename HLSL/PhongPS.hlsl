#include "Phong.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);	// 法線マップ
Texture2D shadowMap : register(t2);	// シャドウマップ

SamplerState diffuseMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 diffuseColor = diffuseMap.Sample(diffuseMapSamplerState, pin.texcoord) * pin.color;

	// 法線マップからxyz成分を取得して( -1 ～ +1 )の間にスケーリング
	float3 normal = normalMap.Sample(diffuseMapSamplerState, pin.texcoord).xyz * 2 - 1;
	float3x3 CM   = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
	float3 N      = normalize(mul(normal, CM));

	float3 L = normalize(directionalLightData.direction.xyz);
	float3 E = normalize(viewPosition.xyz - pin.world_position.xyz);

	// 環境光の計算
	float3 ambient = ka.rgb * ambientLightColor.rgb * diffuseColor.rgb;

	// 平行光源のライティング計算
	float3 diffuse = CalcLambertDiffuse(N, L, directionalLightData.color.rgb, kd.rgb);
	float3 specular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, E, shiness, ks.rgb);

	// 平行光源の影なので、平行光源に対して影を適応
	float3 shadow = CalcShadowColor(shadowMap, shadowMapSamplerState, pin.shadowTexcoord, shadowColor, shadowBias);
	diffuse *= shadow;
	specular *= shadow;

	// 点光源の処理
	int i; 
	for (i = 0; i < pointLightCount; ++i)
	{
		// ライトベクトルを算出
		float3 lightVector = pin.world_position - pointLightData[i].position.xyz;

		// ライトベクトルの長さを算出
		float lightLength = length(lightVector);

		// ライトの影響範囲外なら後の計算をしない。
		if (lightLength >= pointLightData[i].range)
			continue;

		// 距離減衰を算出する
		float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);

		lightVector = lightVector / lightLength;
		diffuse += CalcLambertDiffuse(N, lightVector,
									pointLightData[i].color.rgb, kd.rgb) * attenuate;
		specular += CalcPhongSpecular(N, lightVector,
									pointLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
	}

	// スポットライトの処理
	for (i = 0; i < spotLightCount; ++i)
	{
		// ライトベクトルを算出
		float3 lightVector = pin.world_position - spotLightData[i].position.xyz;

		// ライトベクトルの長さを算出
		float lightLength = length(lightVector);

		if (lightLength >= spotLightData[i].range)
			continue;

		// 距離減衰を算出する
		float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);

		lightVector = normalize(lightVector);

		// 角度減衰を算出してattenuateに乗算する
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
