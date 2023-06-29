#include "WaterSurface.hlsli"
#include "CommonFunctions.hlsli"

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1); // 法線マップ
Texture2D skyboxMap : register(t2); // 空テクスチャ
Texture2D shadowMap : register(t3); // シャドウマップ


SamplerState diffuseMapSamplerState : register(s0);
SamplerState shadowMapSamplerState : register(s1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 波の揺らぎをUV座標に与える
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
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);

        diffuse += CalcLambertDiffuse(N, lightVector,
									spotLightData[i].color.rgb, kd.rgb) * attenuate;
        specular += CalcPhongSpecular(N, lightVector,
									spotLightData[i].color.rgb, E, shiness, ks.rgb) * attenuate;
    }

    float4 color = float4(diffuseColor.rgb * (ambient + diffuse) + specular, diffuseColor.a);

	// 反射色
    float3 reflectColor = 0.0f;
	{
		// 視線を法線で反射した位置の色を取得する
        //float3 R = reflect(-E, float3(0, 1, 0));
        float3 R = reflect(-E, N);

		// 正距円筒図法に則って作成されたパノラマ画像からサンプリングするためのUV座標を算出する
        float2 texcoord = EquirectangularProjection(R);
        reflectColor = skyboxMap.Sample(diffuseMapSamplerState, texcoord).rgb;
    }
    //return float4(color.rgb + reflectColor, color.a);
    
    // shlickの近似式を利用して反射率を算出して、透明度と空テクスチャとのブレンド率を変える
    float n1 = 0.1002f;
    float n2 = 0.1333f;
    float f0 = ((n1 - n2) * (n1 - n2)) / ((n1 + n2) * (n1 + n2));
    
    // フレネル反射率の計算
    float reflectance = f0 + (1 - f0) * pow(1 - cos(dot(E, N)), 5);
    
    // カラーと空テクスチャとをブレンドする
    color.rgb += reflectColor * reflectance;
    
	// 今回は水深を考えないため単純に透明度に適応してしまうとほぼ水底が見えてしまう。
	// なのでテキトーに係数を加算しておく
    color.a *= saturate(reflectance + 0.4f);
    
    return color;

}
