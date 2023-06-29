#pragma once
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite = nullptr;
	std::unique_ptr<Sprite> buttonSprite = nullptr;
	std::unique_ptr<Sprite> loadingSprite = nullptr;
	std::unique_ptr<Texture> title = nullptr;
	std::unique_ptr<Texture> buttonTexture = nullptr;
	std::unique_ptr<Texture> loadingTexture = nullptr;

	float titleEffect = 1.0f;
	bool tooHigh = false;
	bool tooLow = false;

	bool Loading = false;
};