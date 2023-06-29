#pragma once

#include "Renderer/BaseRenderer.h"

// シェーダーリソースビューの情報
struct ShaderResourceViewData
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	color;
	UINT	width = 0;
	UINT	height = 0;
};

// ポストプロセス用のレンダラー
class PostprocessingRenderer : public BaseRenderer
{
	// ブルーム情報
	struct BloomData
	{
		//	高輝度抽出用情報
		LuminanceExtractionData	luminanceExtractionData;
		//	ガウスブラー用情報
		GaussianFilterData		gaussianFilterData;
	};

public:
	PostprocessingRenderer(UINT width, UINT height);
	~PostprocessingRenderer();

	//	描画
	void	Render(ID3D11DeviceContext* dc) override;

	// RenderTargetのShaderResourceViewを取得
	const SRVHandleList GetRenderTargetShaderResourceViews() const override;

	// DepthStencilのShaderResourceViewを取得
	const SRVHandle GetDepthStencilShaderResourceView() const override;

	//	シーン情報の設定
	void	SetSceneData(ShaderResourceViewData srv);

	//	デバッグ情報の表示
	void	DrawDebugGUI();

private:
	// シーン描画データ
	ShaderResourceViewData			sceneData;

	// 描画用スプライト
	std::unique_ptr<Sprite>			renderSprite;

	// ブルームデータ
	BloomData						bloomData;

	// 色調補正データ
	ColorGradingData				colorGradingData;

	//	高輝度抽出テクスチャ
	std::unique_ptr<RenderTarget>	workRenderTarget[2];
};

