#include "PostprocessingRenderer.h"
#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"
#include "Camera.h"

PostprocessingRenderer::PostprocessingRenderer(UINT width, UINT height)
{
	Graphics& graphics = Graphics::Instance();

	renderSprite = std::make_unique<Sprite>();

	// 高輝度抽出用描画ターゲットを生成
	workRenderTarget[0].release();
	workRenderTarget[0] = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	// 高輝度抽出ぼかし用描画ターゲットを生成
	workRenderTarget[1].release();
	workRenderTarget[1] = std::make_unique<RenderTarget>(width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

	// ガウスブラーのテクセルサイズを先に決めておく
	bloomData.gaussianFilterData.textureSize.x = static_cast<float>(workRenderTarget[1]->GetWidth());
	bloomData.gaussianFilterData.textureSize.y = static_cast<float>(workRenderTarget[1]->GetHeight());
}

PostprocessingRenderer::~PostprocessingRenderer()
{
}

void	PostprocessingRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	RenderContext rc;
	rc.deviceContext = dc;
	rc.screenSize.x = graphics.GetScreenWidth();
	rc.screenSize.y = graphics.GetScreenHeight();
	rc.screenSize.z = camera.GetNearZ();
	rc.screenSize.w = camera.GetFarZ();

	// カメラパラメータ設定
	rc.viewPosition.x = camera.GetEye().x;
	rc.viewPosition.y = camera.GetEye().y;
	rc.viewPosition.z = camera.GetEye().z;
	rc.viewPosition.w = 1;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	// ライトの情報をセット
	LightManager::Instane().PushRenderContext(rc);

	// シェーダー用情報の設定
	rc.luminanceExtractionData = bloomData.luminanceExtractionData;
	rc.gaussianFilterData = bloomData.gaussianFilterData;

	//	高輝度抽出用バッファに描画先を変更して高輝度抽出
	{
		//	描画先を変更
		ID3D11RenderTargetView* rtv = workRenderTarget[0]->GetRenderTargetView().Get();
		FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		rc.deviceContext->ClearRenderTargetView(rtv, color);
		rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
		D3D11_VIEWPORT	viewport{};
		viewport.Width = static_cast<float>( workRenderTarget[0]->GetWidth() );
		viewport.Height = static_cast<float>( workRenderTarget[0]->GetHeight() );
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		rc.deviceContext->RSSetViewports(1, &viewport);

		//	高輝度抽出処理
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::LuminanceExtraction);
		shader->Begin(rc);

		//	描画対象を変更
		renderSprite->SetShaderResourceView(sceneData.color, sceneData.width, sceneData.height);
		renderSprite->Update(0, 0, viewport.Width, viewport.Height,
			0, 0, static_cast<float>( sceneData.width ), static_cast<float>( sceneData.height ),
			0,
			1, 1, 1, 1);
		shader->Draw(rc, renderSprite.get());
		shader->End(rc);
	}

	// 抽出した高輝度描画対象を暈して書き込む
	{
		//	描画先を変更
		ID3D11RenderTargetView* rtv = workRenderTarget[1]->GetRenderTargetView().Get();
		FLOAT color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		rc.deviceContext->ClearRenderTargetView(rtv, color);
		rc.deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
		D3D11_VIEWPORT	viewport{};
		viewport.Width = static_cast<float>( workRenderTarget[1]->GetWidth() );
		viewport.Height = static_cast<float>( workRenderTarget[1]->GetHeight() );
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		rc.deviceContext->RSSetViewports(1, &viewport);

		//	高輝度抽出処理
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::GaussianBlur);
		shader->Begin(rc);

		//	描画対象を変更
		renderSprite->SetShaderResourceView(workRenderTarget[0]->GetShaderResourceView(),
			workRenderTarget[0]->GetWidth(),
			workRenderTarget[0]->GetHeight());
		renderSprite->Update(0, 0, viewport.Width, viewport.Height,
			0, 0, static_cast<float>( workRenderTarget[0]->GetWidth() ), static_cast<float>( workRenderTarget[0]->GetHeight() ),
			0,
			1, 1, 1, 1);
		shader->Draw(rc, renderSprite.get());
		shader->End(rc);
	}

	//	元のバッファに戻す
	RestoreRenderTargets(dc);

	// 最終パス
	{
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Finalpass);
		shader->Begin(rc);

		renderSprite->SetShaderResourceView(sceneData.color, sceneData.width, sceneData.height);
		renderSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>( sceneData.width ),static_cast<float>( sceneData.height ),
			0,
			1, 1, 1, 1);

		//	シェーダーに渡す情報を設定
		rc.finalpassnData.bloomTexture = workRenderTarget[1]->GetShaderResourceView().Get();
		rc.finalpassnData.colorMap = sceneData.color.Get();
		rc.colorGradingData = colorGradingData;

		shader->Draw(rc, renderSprite.get());

		shader->End(rc);
	}
}

//	RenderTargetのShaderResourceViewを取得
const SRVHandleList PostprocessingRenderer::GetRenderTargetShaderResourceViews() const
{
	return	SRVHandleList();
}

//	DepthStencilのShaderResourceViewを取得
const SRVHandle PostprocessingRenderer::GetDepthStencilShaderResourceView() const
{
	return	SRVHandle();
}

//	シーン情報の設定
void	PostprocessingRenderer::SetSceneData(ShaderResourceViewData srv)
{
	sceneData	= srv;
}

//	デバッグ情報の表示
void	PostprocessingRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("PostProcess"))
	{
		if (ImGui::TreeNode("Bloom"))
		{
			ImGui::SliderFloat("threshold", &bloomData.luminanceExtractionData.threshold, 0.0f, 1.0f);
			ImGui::SliderFloat("intensity", &bloomData.luminanceExtractionData.intensity, 0.0f, 10.0f);
			ImGui::SliderInt("kernelSize", &bloomData.gaussianFilterData.kernelSize, 1, MaxKernelSize - 1);
			ImGui::SliderFloat("deviation", &bloomData.gaussianFilterData.deviation, 1.0f, 10.0f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ColorGrading"))
		{
			ImGui::SliderFloat("hueShift", &colorGradingData.hueShift, 0.0f, 360.0f);
			ImGui::SliderFloat("saturation", &colorGradingData.saturation, 0.0f, 2.0f);
			ImGui::SliderFloat("brightness", &colorGradingData.brightness, 0.0f, 2.0f);
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}
