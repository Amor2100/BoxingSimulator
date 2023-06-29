#pragma once
#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

class SceneLose : public Scene
{
public:
	SceneLose() {}
	~SceneLose() override {}

	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;

private:
	std::unique_ptr<Sprite> loseSprite = nullptr;
	std::unique_ptr<Texture> loseTexture = nullptr;
};