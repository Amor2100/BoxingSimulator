#include "ModelRenderer.h"

//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^
void ModelRenderer::RegisterRenderModel(Model* model)
{
	// ƒ‚ƒfƒ‹‚ªŠù‚É“o˜^‚³‚ê‚Ä‚¢‚é‚©’²‚×‚é
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it != renderModels.end())
		return;

	renderModels.push_back(model);
}

//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
void ModelRenderer::UnregisterRenderModel(Model* model)
{
	// ƒ‚ƒfƒ‹‚ªŠù‚É“o˜^‚³‚ê‚Ä‚¢‚é‚©’²‚×‚é
	std::vector<Model*>::iterator it = std::find(renderModels.begin(), renderModels.end(), model);
	if (it == renderModels.end())
		return;

	renderModels.erase(it);
}

//	•`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
void ModelRenderer::ClearRenderModel()
{
	renderModels.clear();
}
