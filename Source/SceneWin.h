#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

class SceneWin : public Scene
{
public:
	SceneWin() {}
	~SceneWin() override {}

	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;

private:
	std::unique_ptr<Sprite> winSprite = nullptr;
	std::unique_ptr<Texture> winTexture = nullptr;
};