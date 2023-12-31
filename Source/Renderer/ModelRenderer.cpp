#include "ModelRenderer.h"

//	描画モデルの登録
void ModelRenderer::RegisterRenderModel(Model* model)
{
	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it != renderModels.end())
		return;

	renderModels.push_back(model);
}

//	描画モデルの登録解除
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// モデルが既に登録されているか調べる
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it == renderModels.end())
		return;

	renderModels.erase(it);
}

//	描画モデル登録全解除
void ModelRenderer::ClearRenderModel()
{
	renderModels.clear();
}
