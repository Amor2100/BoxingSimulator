#pragma once

#include "BaseRenderer.h"
#include "Graphics/Model.h"

class ModelRenderer : public BaseRenderer
{
public:
	ModelRenderer(){}
	virtual ~ModelRenderer(){}

public:
	//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^
	void RegisterRenderModel(Model* model);

	//	•`‰æƒ‚ƒfƒ‹‚Ì“o˜^‰ğœ
	void UnregisterRenderModel(Model* model);

	//	•`‰æƒ‚ƒfƒ‹“o˜^‘S‰ğœ
	void ClearRenderModel();

protected:
	std::vector<Model*>	renderModels; // •`‰æƒ‚ƒfƒ‹ƒŠƒXƒg
};
