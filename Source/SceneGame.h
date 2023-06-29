#pragma once

#include "Graphics/Model.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"
#include "Scene.h"

#include "Renderer/ShadowmapRenderer.h"
#include "Renderer/SceneRenderer.h"
#include "PostprocessingRenderer.h"
#include "Boxer.h"
#include "BoxerX.h"
#include "Shield.h"
#include "ShieldBackSide.h"
#include "Clouds.h"
#include "StageRing.h"
#include "StageArena.h"
#include "CameraController.h"

// ゲームシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame(){}

	// 初期化
	void Initialize();

	// 終了化
	void Finalize();

	// 更新処理
	void Update(float elapsedTime);

	// 描画処理
	void Render();

	// 描画処理
	void DrawDebugParameter(Model* model, DirectX::XMFLOAT4X4& transform, const char* label);

private:
	void RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

private:
	Boxer* boxer = nullptr;
	BoxerX* boxerX = nullptr;
	Shield* shield = nullptr;
	Shield* enemyShield = nullptr;
	ShieldBackSide* shieldBackSide = nullptr;
	ShieldBackSide* enemyShieldBackSide = nullptr;
	Clouds* playerClouds = nullptr;
	Clouds* enemyClouds = nullptr;
	CameraController* cameraController = nullptr;

	StageRing* stageRing = nullptr;
	StageArena* stageArena = nullptr;
	std::unique_ptr<ShadowmapRenderer>		shadowmapRenderer;
	std::unique_ptr<SceneRenderer>			sceneRenderer;
	std::unique_ptr<PostprocessingRenderer>	postprocessingRenderer;

	std::unique_ptr<Sprite>		renderSplite;
	std::unique_ptr<Texture>	title;
	std::vector<Model*>			models;
	std::vector<Model*>			playerShieldModels;
	std::vector<Model*>			enemyShieldModels;
	std::vector<Model*>			playerCloudsModels;
	std::vector<Model*>			enemyCloudsModels;

	// How To UI 
	std::unique_ptr<Sprite> howToSprite = nullptr;
	std::unique_ptr<Texture> howToTexture = nullptr;
	bool howToDone = false;
	float howToLeft = 1180;
	float howToRight = 1500;
	float howToTop = 690;
	float howToBot = 830;

	// Select Difficulty UI

	std::unique_ptr<Sprite> statsSprite = nullptr;
	std::unique_ptr<Sprite> numberSprite = nullptr;
	std::unique_ptr<Sprite> difficultySprite = nullptr;
	std::unique_ptr<Sprite> plusHealthSprite = nullptr;
	std::unique_ptr<Sprite> plusDefSprite = nullptr;
	std::unique_ptr<Sprite> plusAtkSprite = nullptr;
	std::unique_ptr<Sprite> plusStaminaSprite = nullptr;
	std::unique_ptr<Sprite> sceneGame = nullptr;
	std::unique_ptr<Sprite> playerHealth = nullptr;
	std::unique_ptr<Sprite> playerStamina = nullptr;

	std::unique_ptr<Texture> statsTexture = nullptr;
	std::unique_ptr<Texture> numberTexture = nullptr;
	std::unique_ptr<Texture> difficultyTexture = nullptr;
	std::unique_ptr<Texture> sceneGameTexture = nullptr;
	std::unique_ptr<Texture> playerHealthTexture = nullptr;
	std::unique_ptr<Texture> playerStaminaTexture = nullptr;

	// 攻撃タイプUI
	std::unique_ptr<Sprite> kougekiType = nullptr;
	std::unique_ptr<Sprite> buttonSprite = nullptr;
	std::unique_ptr<Texture> kougekiTypeTexture = nullptr;
	std::unique_ptr<Texture> buttonTexture = nullptr;
	float titleEffect = 1.0f;
	bool tooHigh = false;
	bool tooLow = false;
	bool typeDone = false;
	int kougekiList = 0;
	// Next Button
	int nextLeftX = 1155;
	int nextRightX = 1500;
	int nextTopY = 705;
	int nextBotY = 805;
	// Back Button
	int backLeftX = 45;
	int backRightX = 705;
	int backTopY = 390;
	int backBotY = 805;


	bool playerUpdateStats = false;
	float screenWidth;
	float screenHeight;

	// Ready Button
	int readyLeftX = 1185;
	int readyRightX = 1475;
	int readyTopY = 700;
	int readyBotY = 815;

	// HP Increase
	int HPupLeftX = 1030;
	int HPupRightX = 1130;
	int HPupTopY = 250;
	int HPupBotY = 345;
	// HP Decrease
	int HPdownLeftX = 895;
	int HPdownRightX = 990;
	int HPdownTopY = 250;
	int HPdownBotY = 345;

	// Reduce Increase
	int RedupLeftX = 1030;
	int RedupRightX = 1130;
	int RedupTopY = 390;
	int RedupBotY = 490;
	// Reduce Decrease
	int ReddownLeftX = 895;
	int ReddownRightX = 990;
	int ReddownTopY = 390;
	int ReddownBotY = 490;

	// Dmg Increase
	int DmgupLeftX = 1030;
	int DmgupRightX = 1130;
	int DmgupTopY = 525;
	int DmgupBotY = 625;
	// Dmg Decrease
	int DmgdownLeftX = 895;
	int DmgdownRightX = 990;
	int DmgdownTopY = 525;
	int DmgdownBotY = 625;

	// Stam Increase
	int StamupLeftX = 1030;
	int StamupRightX = 1130;
	int StamupTopY = 665;
	int StamupBotY = 765;
	// Stam Decrease
	int StamdownLeftX = 895;
	int StamdownRightX = 990;
	int StamdownTopY = 665;
	int StamdownBotY = 765;


	int numberPos = 0;
	int numberSize = 95;

	// STATS
	float plusHealth = 0;
	float plusDef = 0;
	float plusAtk = 0;
	float plusStamina = 0;
	int statsLeft = 10;

	// Difficulty
	int difficulty = 0;
	bool difficultyChosen = false;
	// Easy
	int easyLeftX = 65;
	int easyRightX = 370;
	// Medium
	int mediumLeftX = 610;
	int mediumRightX = 925;
	// Hard
	int hardLeftX = 1135;
	int hardRightX = 1445;


	int TopY = 435;
	int BotY = 745;

	// UI needed
	float stamPlayerUI = 0.0f;
	float healthPlayerUI = 0.0f;

	// Scene Win/Lose Delay
	float sceneChangeDelay = 2.4f;

	// Enemy UI
	std::unique_ptr<Sprite> gauge = nullptr;
	std::unique_ptr<Sprite> baseGauge = nullptr;
	std::unique_ptr<Sprite> stamGauge = nullptr;
	std::unique_ptr<Texture> gaugeTexture = nullptr;
	std::unique_ptr<Texture> baseGaugeTexture = nullptr;
	std::unique_ptr<Texture> stamGaugeTexture = nullptr;

	// Start UI
	std::unique_ptr<Sprite> startUISprite = nullptr;
	std::unique_ptr<Texture> startUITexture = nullptr;
	float sceneUITimer = 3.0f;
	int sceneUITimerIndicator = 0;

	// End UI
	std::unique_ptr<Sprite> endUISprite = nullptr;
	std::unique_ptr<Texture> endUITexture = nullptr;

	// Stunned / Shield UI
	std::unique_ptr<Sprite> playerNotif = nullptr;
	std::unique_ptr<Sprite> enemyNotif = nullptr;
	std::unique_ptr<Texture> notifTexture = nullptr;
	float playerNotifDelay = 0.6f;
	float enemyNotifDelay = 0.6f;
	DirectX::XMFLOAT3 light3dir = { 0.3f, -1.0f, 0.3f };
	bool changeDirX = false;
	bool changeDirZ = false;
};
