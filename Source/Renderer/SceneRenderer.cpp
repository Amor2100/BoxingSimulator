#include "SceneRenderer.h"

#include "Graphics/LightManager.h"
#include "Camera.h"
#include "EffectManager.h"
SceneRenderer::SceneRenderer(UINT width, UINT height)
{
	renderTarget.release();
	depthStencil.release();

	renderTarget = std::make_unique<RenderTarget>( width, height, DXGI_FORMAT_R8G8B8A8_UNORM );
	depthStencil = std::make_unique<DepthStencil>( width, height );

	// 空描画用スプライト
	skyboxRenderer = std::make_unique<Sprite>();

	// ライトデバッグフラグ
	drawDebugPrimitive	= false;
}

void SceneRenderer::Render(ID3D11DeviceContext* dc)
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();

	// 現在設定されているバッファを退避
	CacheRenderTargets(dc);

	// 画面クリア＆レンダーターゲット設定
	ID3D11RenderTargetView* rtv = renderTarget->GetRenderTargetView().Get();
	ID3D11DepthStencilView* dsv = depthStencil->GetDepthStencilView().Get();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// ビューポートの設定
	D3D11_VIEWPORT	vp = {};
	vp.Width = static_cast<float>(renderTarget->GetWidth());
	vp.Height = static_cast<float>(renderTarget->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dc->RSSetViewports(1, &vp);

	// 描画コンテキストに情報をセット
	RenderContext rc;
	{
		rc.deviceContext = dc;
		rc.timer = ++timer;
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

		// シャドウマップの情報をセット
		rc.shadowMapData = shadowMapData;
	}

	// 空を描画
	const std::unique_ptr<Texture>& skyboxTexture = LightManager::Instane().GetSkyboxTexture();
	if (skyboxTexture)
	{
		// 空テクスチャを設定
		skyboxRenderer->SetShaderResourceView(skyboxTexture->GetShaderResourceView().Get(),
			skyboxTexture->GetWidth(),
			skyboxTexture->GetHeight());
		skyboxRenderer->Update(0, 0, vp.Width, vp.Height,
			0, 0, static_cast<float>(skyboxTexture->GetWidth()), static_cast<float>(skyboxTexture->GetHeight()),
			0, 1, 1, 1, 1);
		// 空描画
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Skybox);
		shader->Begin(rc);
		shader->Draw(rc, skyboxRenderer.get());
		shader->End(rc);
	}


	// 登録モデルを描画
	ModelShaderId shaderId = static_cast<ModelShaderId>(0xFFFFFFFF);
	ModelShader* shader = nullptr;
	for (const Model* model : renderModels)
	{
		const ModelResource* resource = model->GetResource();
		const std::vector<Model::Node>& nodes = model->GetNodes();

		for (const ModelResource::Mesh& mesh : resource->GetMeshes())
		{
			bool updateBuffers = true;
			for (const ModelResource::Subset& subset : mesh.subsets)
			{
				//	シェーダー設定
				ModelShaderId currentShaderId = static_cast<ModelShaderId>(subset.material->shaderId);
				if (shaderId != currentShaderId)
				{
					//	シェーダー変更時の終了処理呼び出し
					if (shader)
						shader->End(rc);

					//	シェーダー変更
					shaderId = currentShaderId;
					if (currentShaderId == ModelShaderId::Max)
						continue;
					shader = graphics.GetShader(currentShaderId);

					//	レンダーステート等の初回設定
					shader->Begin(rc);
					updateBuffers = true;
				}

				//	頂点・インデックスバッファ等設定
				if (updateBuffers)
				{
					shader->SetBuffers(rc, nodes, mesh);
					updateBuffers = false;
				}

				//	サブセット単位で描画
				shader->DrawSubset(rc, subset);
			}
		}
	}
	//	シェーダーがあれば終了処理
	if (shader)
		shader->End(rc);

	EffectManager::Instance().Render(camera.GetView(), camera.GetProjection());

	//	デバッグプリミティブの表示
	if(drawDebugPrimitive)
	{
		// ライトのデバッグプリミティブ描画
		LightManager::Instane().DrawDebugPrimitive();

		// ラインレンダラ描画実行
		graphics.GetLineRenderer()->Render(rc.deviceContext, rc.view, rc.projection);

		// デバッグレンダラ描画実行
		graphics.GetDebugRenderer()->Render(rc.deviceContext, rc.view, rc.projection);
	}

	//	元のバッファに戻す
	RestoreRenderTargets(dc);
}

//	デバッグGUI描画
void SceneRenderer::DrawDebugGUI()
{
	if (ImGui::TreeNode("Scene Rendering"))
	{
		ImGui::Checkbox("DebugPrimitive", &drawDebugPrimitive);

		if (ImGui::TreeNode("RenderTargets"))
		{
			ImGui::Text("Color");
			ImGui::Image(renderTarget->GetShaderResourceView().Get(), {256, 256}, {0, 0}, {1, 1}, {1, 1, 1, 1} );
			ImGui::Text("Depth");
			ImGui::Image(depthStencil->GetShaderResourceView().Get(), {256, 256}, {0, 0}, {1, 1}, {1, 1, 1, 1} );
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}

//	RenderTargetのShaderResourceViewを取得
const SRVHandleList SceneRenderer::GetRenderTargetShaderResourceViews() const
{
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>	list;
	list.push_back(renderTarget->GetShaderResourceView());
	return	list;
}

//	DepthStencilのShaderResourceViewを取得
const SRVHandle SceneRenderer::GetDepthStencilShaderResourceView() const
{
	return	depthStencil->GetShaderResourceView();
}
