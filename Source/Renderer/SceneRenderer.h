#pragma once

#include "Renderer/ModelRenderer.h"

// シーンレンダラー
class SceneRenderer : public ModelRenderer
{
public:
	SceneRenderer(UINT width, UINT height);
	~SceneRenderer(){}

	// シャドウマップ情報をRenderContextに積む
	void SetShadowmapData(ShadowMapData shadowMapData) { this->shadowMapData = shadowMapData; }

public:
	//	描画実行
	void Render(ID3D11DeviceContext* dc) override;

	//	デバッグGUI描画
	void DrawDebugGUI() override;

	//	RenderTargetのShaderResourceViewを取得
	const SRVHandleList GetRenderTargetShaderResourceViews() const override;

	//	DepthStencilのShaderResourceViewを取得
	const SRVHandle GetDepthStencilShaderResourceView() const override;

private:
	std::unique_ptr<RenderTarget>	renderTarget;		//	オフスクリーンレンダリング用描画バッファ
	std::unique_ptr<DepthStencil>	depthStencil;		//	オフスクリーンレンダリング用深度バッファ
	std::unique_ptr<Sprite>			skyboxRenderer;		//	空描画用スプライト
	ShadowMapData					shadowMapData;		//	シャドウマップ情報
	bool							drawDebugPrimitive;	//	デバッグプリミティブ描画
	int timer;
};
