#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

//	平行光源情報
struct DirectionalLightData
{
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
};

// 点光源情報
struct PointLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	DirectX::XMFLOAT3	dummy;
};

// 点光源の最大数
static	constexpr	int	PointLightMax = 8;

// スポットライト情報
struct SpotLightData
{
	DirectX::XMFLOAT4	position;	// 座標
	DirectX::XMFLOAT4	direction;	// 向き
	DirectX::XMFLOAT4	color;		// 色
	float				range;		// 範囲
	float				innerCorn; 	// インナー角度範囲
	float				outerCorn; 	// アウター角度範囲
	float				dummy;
};

// スポットライトの最大数
static	constexpr	int	SpotLightMax = 8;

// 色調補正情報
struct ColorGradingData
{
	float	hueShift	= 0;	// 色相調整
	float	saturation	= 1;	// 彩度調整
	float	brightness	= 1;	// 明度調整
	float	dummy;
};

// ガウスフィルター計算情報
struct GaussianFilterData
{
	int					kernelSize	= 15;		// カーネルサイズ
	float				deviation	= 10.0f;	// 標準偏差
	DirectX::XMFLOAT2	textureSize;			// 暈すテクスチャのサイズ
};

// ガウスフィルターの最大カーネルサイズ
static const int MaxKernelSize = 16;

// 高輝度抽出用情報
struct LuminanceExtractionData
{
	float				threshold	= 0.514f;	// 閾値
	float				intensity	= 5.015f;	// ブルームの強度
	DirectX::XMFLOAT2	dummy2;
};

// ポストエフェクトの最終パス用情報
struct FinalpassnData
{
	//	ブルームテクスチャ
	ID3D11ShaderResourceView* bloomTexture;
	//	カラーテクスチャ
	ID3D11ShaderResourceView* colorMap;
};

//	シャドウマップ用情報
struct ShadowMapData
{
	ID3D11ShaderResourceView*	shadowMap;				//	シャドウマップテクスチャ
	DirectX::XMFLOAT4X4			lightViewProjection;	//	ライトビュープロジェクション行列
	DirectX::XMFLOAT3			shadowColor;			//	影の色
	float						shadowBias;				//	深度比較用のオフセット値
};

//	スカイボックス用情報
struct SkyboxData
{
	ID3D11ShaderResourceView* skybox;	//	空テクスチャ
};


// レンダーコンテキスト
struct RenderContext
{
	ID3D11DeviceContext*		deviceContext;
	float timer;
	DirectX::XMFLOAT4			screenSize;	//	xy : スクリーンのサイズ, z : nearクリップ距離、w : farクリップ距離

	//	カメラ情報
	DirectX::XMFLOAT4			viewPosition;
	DirectX::XMFLOAT4X4			view;
	DirectX::XMFLOAT4X4			projection;

	//	ライト情報
	DirectX::XMFLOAT4			ambientLightColor;				// 環境光情報
	DirectionalLightData		directionalLightData;			// 平行光源情報
	PointLightData				pointLightData[PointLightMax];	// 点光源情報
	SpotLightData				spotLightData[SpotLightMax];	// スポットライト情報
	int							pointLightCount = 0;			// 点光源数
	int							spotLightCount = 0;				// スポットライト数

	//	色調補正情報
	ColorGradingData			colorGradingData;

	//	ガウスフィルター情報
	GaussianFilterData			gaussianFilterData;

	//	高輝度抽出用情報
	LuminanceExtractionData		luminanceExtractionData;

	//	最終パス情報
	FinalpassnData				finalpassnData;

	//	シャドウマップ情報
	ShadowMapData				shadowMapData;

	//	スカイボックス情報
	SkyboxData					skyboxData;

};
