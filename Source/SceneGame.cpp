#include "Graphics/Graphics.h"
#include "Graphics/LightManager.h"
#include "Input/Input.h"
#include "SceneGame.h"
#include "Camera.h"
#include "StageManager.h"
#include "SceneManager.h"
#include "SceneWin.h"
#include "SceneLose.h"
#include "EnemyManager.h"
#include "EffectManager.h"

// 初期化
void SceneGame::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	StageManager& stageManager = StageManager::Instance();
	boxer					= new Boxer();
	boxerX					= new BoxerX();
	shield					= new Shield();
	enemyShield				= new Shield();
	shieldBackSide			= new ShieldBackSide();
	enemyShieldBackSide		= new ShieldBackSide();
	playerClouds			= new Clouds();
	enemyClouds				= new Clouds();
	cameraController		= new CameraController();
	EffectManager::Instance().Initialize();

	stageRing = new StageRing();
	stageArena = new StageArena();
	stageManager.Register(stageRing);
	stageManager.Register(stageArena);

	EnemyManager& enemyManager = EnemyManager::Instance();
	boxerX->SetPosition(DirectX::XMFLOAT3(0, 0, -3));
	EnemyManager::Instance().Register(boxerX);

	//	各種レンダラー生成
	{
		UINT width = static_cast<UINT>(graphics.GetScreenWidth());
		UINT height = static_cast<UINT>(graphics.GetScreenHeight());
		//	シャドウマップレンダラー
		shadowmapRenderer	= std::make_unique<ShadowmapRenderer>(2048);
		//	シーンレンダラー
		sceneRenderer		= std::make_unique<SceneRenderer>(width, height);
		//	ポストプロセスレンダラー
		postprocessingRenderer	= std::make_unique<PostprocessingRenderer>(width / 2, height / 2);
		const SRVHandleList	list	= sceneRenderer->GetRenderTargetShaderResourceViews();
		postprocessingRenderer->SetSceneData({list[0], width, height});
	}

	//	モデル登録
	{
		//	モデルを各レンダラーに登録
		models.emplace_back(boxer->model.get());
		models.emplace_back(boxerX->model.get());
		models.emplace_back(stageRing->model.get());
		models.emplace_back(stageArena->model.get());
		
		for (Model* model : models){
			shadowmapRenderer->RegisterRenderModel(model);
			sceneRenderer->RegisterRenderModel(model);

			// ついでにデフォルトのシェーダーを変更する
			// 本来はモデルエディターなどを自作して、モデル単位で事前に変えておくと良い
			const ModelResource* resource = model->GetResource();
			for (const ModelResource::Material& material : resource->GetMaterials()){
				ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
				mat.shaderId = static_cast<int>(ModelShaderId::Phong);
			}
		}
		models.clear();
	}

	// 平行光源を追加
	{
		Light*	light = new Light(LightType::Directional);
		light->SetDirection({ 1, -1, -1 });
		LightManager::Instane().Register(light);
		
		// シャドウマップ用の光源として指定する
		LightManager::Instane().SetShadowmapLight(light);
	}

	// スポットライト
	for( int i = 0; i < 1; ++i )
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition({0, 15, 0});
		light->SetRange(450);
		light->SetInnerCorn(0.99f);
		light->SetOuterCorn(0.98f);
		light->SetDirection({0, -1, 0});
		light->SetColor({0, 0, 1, 1.0f});
		LightManager::Instane().Register(light);
	}
	for (int i = 0; i < 1; ++i)
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition({ 0, 15, 0 });
		light->SetRange(450);
		light->SetInnerCorn(0.99f);
		light->SetOuterCorn(0.98f);
		light->SetDirection({ 0, -1, 0 });
		light->SetColor({ 1, 0, 0, 1.0f });
		LightManager::Instane().Register(light);
	}

	// 2D画像を表示する
	{
		// Difficulty Select & Player Stats
		statsSprite			= std::make_unique<Sprite>();
		numberSprite		= std::make_unique<Sprite>();
		difficultySprite	= std::make_unique<Sprite>();
		plusHealthSprite	= std::make_unique<Sprite>();
		plusDefSprite		= std::make_unique<Sprite>();
		plusAtkSprite		= std::make_unique<Sprite>();
		plusStaminaSprite	= std::make_unique<Sprite>();
		gauge				= std::make_unique<Sprite>();
		baseGauge			= std::make_unique<Sprite>();
		stamGauge			= std::make_unique<Sprite>();
		sceneGame			= std::make_unique<Sprite>();
		playerHealth		= std::make_unique<Sprite>();
		playerStamina		= std::make_unique<Sprite>();
		startUISprite		= std::make_unique<Sprite>();
		endUISprite			= std::make_unique<Sprite>();
		playerNotif			= std::make_unique<Sprite>();
		enemyNotif			= std::make_unique<Sprite>();
		kougekiType			= std::make_unique<Sprite>();
		buttonSprite		= std::make_unique<Sprite>();
		howToSprite		= std::make_unique<Sprite>();

		gaugeTexture			= std::make_unique<Texture>("Data/Texture/red.png");
		baseGaugeTexture		= std::make_unique<Texture>("Data/Texture/grey.png");
		stamGaugeTexture		= std::make_unique<Texture>("Data/Texture/blue.png");
		sceneGameTexture		= std::make_unique<Texture>("Data/Texture/whitefont.png");
		playerHealthTexture		= std::make_unique<Texture>("Data/Texture/playerhealth.png");
		playerStaminaTexture	= std::make_unique<Texture>("Data/Texture/playerstamina.png");
		statsTexture			= std::make_unique<Texture>("Data/Texture/laststats.png");
		numberTexture			= std::make_unique<Texture>("Data/Texture/0-25.png");
		difficultyTexture		= std::make_unique<Texture>("Data/Texture/difficulty.png");
		startUITexture			= std::make_unique<Texture>("Data/Texture/fullstartui.png");
		endUITexture			= std::make_unique<Texture>("Data/Texture/ko.png");
		notifTexture			= std::make_unique<Texture>("Data/Texture/fullnotif.png");
		kougekiTypeTexture		= std::make_unique<Texture>("Data/Texture/kougekifull.png");
		buttonTexture			= std::make_unique<Texture>("Data/Texture/continuebutton.png");
		howToTexture			= std::make_unique<Texture>("Data/Texture/sousahouhoured.png");

		statsSprite				->SetShaderResourceView(statsTexture->GetShaderResourceView(), statsTexture->GetWidth(), statsTexture->GetHeight());
		numberSprite			->SetShaderResourceView(numberTexture->GetShaderResourceView(), numberTexture->GetWidth(), numberTexture->GetHeight());
		difficultySprite		->SetShaderResourceView(difficultyTexture->GetShaderResourceView(), difficultyTexture->GetWidth(), difficultyTexture->GetHeight());
		plusHealthSprite		->SetShaderResourceView(numberTexture->GetShaderResourceView(), numberTexture->GetWidth(), numberTexture->GetHeight());
		plusDefSprite			->SetShaderResourceView(numberTexture->GetShaderResourceView(), numberTexture->GetWidth(), numberTexture->GetHeight());
		plusAtkSprite			->SetShaderResourceView(numberTexture->GetShaderResourceView(), numberTexture->GetWidth(), numberTexture->GetHeight());
		plusStaminaSprite		->SetShaderResourceView(numberTexture->GetShaderResourceView(), numberTexture->GetWidth(), numberTexture->GetHeight());
		sceneGame				->SetShaderResourceView(sceneGameTexture->GetShaderResourceView(), sceneGameTexture->GetWidth(), sceneGameTexture->GetHeight());
		playerHealth			->SetShaderResourceView(playerHealthTexture->GetShaderResourceView(), playerHealthTexture->GetWidth(), playerHealthTexture->GetHeight());
		playerStamina			->SetShaderResourceView(playerStaminaTexture->GetShaderResourceView(), playerStaminaTexture->GetWidth(), playerStaminaTexture->GetHeight());
		startUISprite			->SetShaderResourceView(startUITexture->GetShaderResourceView(), startUITexture->GetWidth(), startUITexture->GetHeight());
		endUISprite				->SetShaderResourceView(endUITexture->GetShaderResourceView(), endUITexture->GetWidth(), endUITexture->GetHeight());
		playerNotif				->SetShaderResourceView(notifTexture->GetShaderResourceView(), notifTexture->GetWidth(), notifTexture->GetHeight());
		enemyNotif				->SetShaderResourceView(notifTexture->GetShaderResourceView(), notifTexture->GetWidth(), notifTexture->GetHeight());
		kougekiType				->SetShaderResourceView(kougekiTypeTexture->GetShaderResourceView(), kougekiTypeTexture->GetWidth(), kougekiTypeTexture->GetHeight());
		buttonSprite			->SetShaderResourceView(buttonTexture->GetShaderResourceView(), buttonTexture->GetWidth(), buttonTexture->GetHeight());
		howToSprite				->SetShaderResourceView(howToTexture->GetShaderResourceView(), howToTexture->GetWidth(), howToTexture->GetHeight());
		gauge					->SetShaderResourceView(gaugeTexture->GetShaderResourceView(), 120, 40);
		baseGauge				->SetShaderResourceView(baseGaugeTexture->GetShaderResourceView(), 120, 40);
		stamGauge				->SetShaderResourceView(stamGaugeTexture->GetShaderResourceView(), 120, 40);
	}
}

// 終了化
void SceneGame::Finalize()
{
	shadowmapRenderer	->ClearRenderModel();
	sceneRenderer		->ClearRenderModel();
	LightManager::Instane().Clear();
}

// 更新処理
void SceneGame::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	screenWidth = graphics.GetScreenWidth();
	screenHeight = graphics.GetScreenHeight();
	Mouse& mouse = Input::Instance().GetMouse();
	POINT currentMousePos;
	GetCursorPos(&currentMousePos);
	if (typeDone)
	{
		switch (playerUpdateStats)
		{
		case true:
			ShowCursor(FALSE);
			DirectX::XMFLOAT3 target = boxer->GetPosition();
			playerClouds->SetShieldPosition(boxer->GetPosition());
			enemyClouds->SetShieldPosition(boxerX->GetPosition());
			shield->SetShieldPosition(boxer->GetPosition());
			shieldBackSide->SetShieldPosition(boxer->GetPosition());
			enemyShield->SetShieldPosition(boxerX->GetPosition());
			enemyShieldBackSide->SetShieldPosition(boxerX->GetPosition());

			DirectX::XMFLOAT3 targetEnemy = boxerX->GetPosition();
			LightManager::Instane().GetLight(1)->SetPosition(DirectX::XMFLOAT3(target.x, target.y + 3.0f, target.z));
			LightManager::Instane().GetLight(2)->SetPosition(DirectX::XMFLOAT3(targetEnemy.x, targetEnemy.y + 3.0f, targetEnemy.z));
			target.y += 1.7f;
			targetEnemy.y += 1.7f;
			cameraController->GetStats(playerUpdateStats);
			cameraController->SetTarget(target);
			cameraController->SetEnemy(targetEnemy);
			cameraController->Update(elapsedTime);
			cameraController->GetHit(boxer->GetCameraShake());

			boxerX->SetPlayerAttackState(boxer->GetPlayerAttackState());
			boxerX->SetPlayerBlockState(boxer->GetPlayerBlockState());
			boxerX->SetPlayerHitFailedState(boxer->GetPlayerHitFailedState());
			boxerX->SetPlayerKnockedState(boxer->GetPlayerKnockedState());
			boxerX->SetEnemyRandomNumber(boxer->GetPlayerRandomNumber());
			boxerX->SetPlayerDefense(boxer->GetPlayerDefense());

			boxer->SetEnemyAttackState(boxerX->GetEnemyAttackState());
			boxer->SetEnemyBlockState(boxerX->GetEnemyBlockState());
			boxer->SetEnemyHitFailedState(boxerX->GetEnemyHitFailedState());
			boxer->SetEnemyKnockedState(boxerX->GetEnemyKnockedState());
			boxer->SetEnemyDefense(boxerX->GetEnemyDefense());
			sceneUITimer -= 0.01f;

			// CHANGE SCENE
			if (boxer->GetPlayerKnockedState() == true) {
				sceneChangeDelay -= 0.01f;
				if (sceneChangeDelay <= 0.0f) {
					SceneManager::Instance().ChangeScene(new SceneLose);
				}
			}
			if (boxerX->GetEnemyKnockedState() == true) {
				sceneChangeDelay -= 0.01f;
				if (sceneChangeDelay <= 0.0f) {
					SceneManager::Instance().ChangeScene(new SceneWin);
				}
			}
			//-----------------------------------------------
			//					SHIELD
			//-----------------------------------------------
			if (boxer->GetPlayerBlockState()) {
				playerShieldModels.emplace_back(shield->model.get());
				playerShieldModels.emplace_back(shieldBackSide->model.get());
			}
			for (Model* model : playerShieldModels) {
				shadowmapRenderer->RegisterRenderModel(model);
				sceneRenderer->RegisterRenderModel(model);

				// ついでにデフォルトのシェーダーを変更する
				// 本来はモデルエディターなどを自作して、モデル単位で事前に変えておくと良い
				const ModelResource* resource = model->GetResource();
				for (const ModelResource::Material& material : resource->GetMaterials()) {
					ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
					mat.shaderId = static_cast<int>(ModelShaderId::Phong);
				}
				if (!boxer->GetPlayerBlockState()) {
					shadowmapRenderer->UnregisterRenderModel(model);
					sceneRenderer->UnregisterRenderModel(model);
				}
			}
			if (boxerX->GetEnemyBlockState()) {
				enemyShieldModels.emplace_back(enemyShield->model.get());
				enemyShieldModels.emplace_back(enemyShieldBackSide->model.get());
			}
			for (Model* model : enemyShieldModels) {
				shadowmapRenderer->RegisterRenderModel(model);
				sceneRenderer->RegisterRenderModel(model);

				// ついでにデフォルトのシェーダーを変更する
				// 本来はモデルエディターなどを自作して、モデル単位で事前に変えておくと良い
				const ModelResource* resource = model->GetResource();
				for (const ModelResource::Material& material : resource->GetMaterials()) {
					ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
					mat.shaderId = static_cast<int>(ModelShaderId::Phong);
				}
				if (!boxerX->GetEnemyBlockState()) {
					shadowmapRenderer->UnregisterRenderModel(model);
					sceneRenderer->UnregisterRenderModel(model);
				}
			}
			//-----------------------------------------------
			//					CLOUDS
			//-----------------------------------------------
			if (boxerX->GetStunedState()) {
				enemyCloudsModels.emplace_back(enemyClouds->model.get());
				enemyNotif->Update(0, 0, screenWidth, screenHeight, (1536 * 0), 0, 1536, static_cast<float>(notifTexture->GetHeight()), 0, 1, 1, 1, 1);
			}
			for (Model* model : enemyCloudsModels) {
				shadowmapRenderer->RegisterRenderModel(model);
				sceneRenderer->RegisterRenderModel(model);

				// ついでにデフォルトのシェーダーを変更する
				// 本来はモデルエディターなどを自作して、モデル単位で事前に変えておくと良い
				const ModelResource* resource = model->GetResource();
				for (const ModelResource::Material& material : resource->GetMaterials()) {
					ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
					mat.shaderId = static_cast<int>(ModelShaderId::Phong);
				}
				if (!boxerX->GetStunedState()) {
					shadowmapRenderer->UnregisterRenderModel(model);
					sceneRenderer->UnregisterRenderModel(model);
				}
			}
			if (boxer->GetStunedState()) {
				playerCloudsModels.emplace_back(playerClouds->model.get());
				playerNotif->Update(0, 0, screenWidth, screenHeight, (1536 * 2), 0, 1536, static_cast<float>(notifTexture->GetHeight()), 0, 1, 1, 1, 1);
			}
			for (Model* model : playerCloudsModels) {
				shadowmapRenderer->RegisterRenderModel(model);
				sceneRenderer->RegisterRenderModel(model);

				// ついでにデフォルトのシェーダーを変更する
				// 本来はモデルエディターなどを自作して、モデル単位で事前に変えておくと良い
				const ModelResource* resource = model->GetResource();
				for (const ModelResource::Material& material : resource->GetMaterials()) {
					ModelResource::Material& mat = const_cast<ModelResource::Material&>(material);
					mat.shaderId = static_cast<int>(ModelShaderId::Phong);
				}
				if (!boxer->GetStunedState()) {
					shadowmapRenderer->UnregisterRenderModel(model);
					sceneRenderer->UnregisterRenderModel(model);
				}
			}
			stamPlayerUI = static_cast<float>(boxer->GetStamina()) / static_cast<float>(boxer->GetMaxStamina());
			healthPlayerUI = static_cast<float>(boxer->GetHealth()) / static_cast<float>(boxer->GetMaxHealth());
			//-----------------------------------------------
			// SHIELD BROKEN
			//-----------------------------------------------
			if (boxer->GetShieldBroke()) {
				playerNotif->Update(0, 0, screenWidth, screenHeight, (1536 * 3), 0, 1536, static_cast<float>(notifTexture->GetHeight()), 0, 1, 1, 1, 1);
			}
			if (boxerX->GetShieldBroke()) {
				enemyNotif->Update(0, 0, screenWidth, screenHeight, (1536 * 1), 0, 1536, static_cast<float>(notifTexture->GetHeight()), 0, 1, 1, 1, 1);
			}

			// SPRITE
			playerHealth->Update(632, 753, 321 * healthPlayerUI, 34, 0, 0, 321, 34, 0, 1, 1, 1, 1);
			playerStamina->Update(632, 809, 321 * stamPlayerUI, 34, 0, 0, 321, 34, 0, 1, 1, 1, 1);
			sceneGame->Update(0, 0, screenWidth, screenHeight, 0, 0, static_cast<float>(sceneGameTexture->GetWidth()), static_cast<float>(sceneGameTexture->GetHeight()), 0, 1, 1, 1, 1);
			if (sceneUITimer >= (2.4f) && sceneUITimer < (3.0f)) {
				sceneUITimerIndicator = 0;
			}
			else if (sceneUITimer >= (1.8f) && sceneUITimer < (2.4f)) {
				sceneUITimerIndicator = 1;
			}
			else if (sceneUITimer >= (1.2f) && sceneUITimer < (1.8f)) {
				sceneUITimerIndicator = 2;
			}
			else if (sceneUITimer >= (0.6f) && sceneUITimer < (1.2f)) {
				sceneUITimerIndicator = 3;
			}
			else if (sceneUITimer >= (0.0f) && sceneUITimer < (0.6f)) {
				sceneUITimerIndicator = 4;
			}
			startUISprite->Update(0, 0, screenWidth, screenHeight, (1536 * sceneUITimerIndicator), 0, 1536, static_cast<float>(startUITexture->GetHeight()), 0, 1, 1, 1, 1);
			endUISprite->Update(0, 0, screenWidth, screenHeight, 0, 0, static_cast<float>(endUITexture->GetWidth()), static_cast<float>(endUITexture->GetHeight()), 0, 1, 1, 1, 1);

			boxer				->Update(elapsedTime);
			boxerX				->Update(elapsedTime);
			playerClouds		->Update(elapsedTime);
			enemyClouds			->Update(elapsedTime);
			shield				->Update(elapsedTime);
			enemyShield			->Update(elapsedTime);
			shieldBackSide		->Update(elapsedTime);
			enemyShieldBackSide	->Update(elapsedTime);
			StageManager::Instance().Update(elapsedTime);
			EffectManager::Instance().Update(elapsedTime);
			break;

		case false:
			// UI POS
			statsSprite			->Update(0, 0, screenWidth, screenHeight, 0, 0, static_cast<float>(statsTexture->GetWidth()), static_cast<float>(statsTexture->GetHeight()), 0, 1, 1, 1, 1);
			numberSprite		->Update(1265, 185, numberSize, numberSize, (numberSize * statsLeft), (numberSize * statsLeft), numberSize, numberSize, 0, 1, 1, 1, 1);
			difficultySprite	->Update(0, 0, screenWidth, screenHeight, 0, 0, static_cast<float>(difficultyTexture->GetWidth()), static_cast<float>(difficultyTexture->GetHeight()), 0, 1, 1, 1, 1);
			plusHealthSprite	->Update(750, 250, numberSize, numberSize, (numberSize * plusHealth), (numberSize * plusHealth), numberSize, numberSize, 0, 1, 1, 1, 1);
			plusDefSprite		->Update(750, 390, numberSize, numberSize, (numberSize * plusDef), (numberSize * plusDef), numberSize, numberSize, 0, 1, 1, 1, 1);
			plusAtkSprite		->Update(750, 525, numberSize, numberSize, (numberSize * plusAtk), (numberSize * plusAtk), numberSize, numberSize, 0, 1, 1, 1, 1);
			plusStaminaSprite	->Update(750, 665, numberSize, numberSize, (numberSize * plusStamina), (numberSize * plusStamina), numberSize, numberSize, 0, 1, 1, 1, 1);

			if (difficultyChosen) {
				// Health Up
				if (currentMousePos.x > HPupLeftX && currentMousePos.y > HPupTopY && currentMousePos.x < HPupRightX && currentMousePos.y < HPupBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (statsLeft != 0) {
							plusHealth += 1;
							statsLeft -= 1;
						}
					}
				}
				// Health Down
				if (currentMousePos.x > HPdownLeftX && currentMousePos.y > HPdownTopY && currentMousePos.x < HPdownRightX && currentMousePos.y < HPdownBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (plusHealth != 0) {
							plusHealth -= 1;
							statsLeft += 1;
						}
					}
				}

				// Reduce Up
				if (currentMousePos.x > RedupLeftX && currentMousePos.y > RedupTopY && currentMousePos.x < RedupRightX && currentMousePos.y < RedupBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (statsLeft != 0) {
							plusDef += 1;
							statsLeft -= 1;
						}
					}
				}
				// Reduce Down
				if (currentMousePos.x > ReddownLeftX && currentMousePos.y > ReddownTopY && currentMousePos.x < ReddownRightX && currentMousePos.y < ReddownBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (plusDef != 0) {
							plusDef -= 1;
							statsLeft += 1;
						}
					}
				}

				// Atk Up
				if (currentMousePos.x > DmgupLeftX && currentMousePos.y > DmgupTopY && currentMousePos.x < DmgupRightX && currentMousePos.y < DmgupBotY)
				{
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT)
					{
						if (statsLeft != 0)
						{
							plusAtk += 1;
							statsLeft -= 1;
						}
					}
				}
				// Atk Down
				if (currentMousePos.x > DmgdownLeftX && currentMousePos.y > DmgdownTopY && currentMousePos.x < DmgdownRightX && currentMousePos.y < DmgdownBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (plusAtk != 0) {
							plusAtk -= 1;
							statsLeft += 1;
						}
					}
				}

				// Stamina Up
				if (currentMousePos.x > StamupLeftX && currentMousePos.y > StamupTopY && currentMousePos.x < StamupRightX && currentMousePos.y < StamupBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (statsLeft != 0) {
							plusStamina += 1;
							statsLeft -= 1;
						}
					}
				}
				// Stamina Down
				if (currentMousePos.x > StamdownLeftX && currentMousePos.y > StamdownTopY && currentMousePos.x < StamdownRightX && currentMousePos.y < StamdownBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (plusStamina != 0) {
							plusStamina -= 1;
							statsLeft += 1;
						}
					}
				}

				// Ready Button
				if (currentMousePos.x > readyLeftX && currentMousePos.y > readyTopY && currentMousePos.x < readyRightX && currentMousePos.y < readyBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						boxer->SetPlayerInfo(plusHealth, plusDef, plusAtk, plusStamina);
						boxerX->SetEnemyInfo((rand() % 2 + 2 + (difficulty)), (rand() % 2 + 2 + (difficulty)), (rand() % 2 + 2 + (difficulty)), (rand() % 2) + 2 + (difficulty));
						boxerX->SetEnemyDifficulty(difficulty);
						SetCursorPos(screenWidth / 2, screenHeight / 2);
						playerUpdateStats = true;
					}
				}
			}
			else
			{
				// Easy
				if (currentMousePos.x > easyLeftX && currentMousePos.y > TopY && currentMousePos.x < easyRightX && currentMousePos.y < BotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (!difficultyChosen) {
							difficulty = 0;
							difficultyChosen = true;
						}
					}
				}
				// Medium
				if (currentMousePos.x > mediumLeftX && currentMousePos.y > TopY && currentMousePos.x < mediumRightX && currentMousePos.y < BotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (!difficultyChosen) {
							difficulty = 1;
							difficultyChosen = true;
						}
					}
				}
				// Hard
				if (currentMousePos.x > hardLeftX && currentMousePos.y > TopY && currentMousePos.x < hardRightX && currentMousePos.y < BotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						if (!difficultyChosen) {
							difficulty = 2;
							difficultyChosen = true;
						}
					}
				}
			}
			break;
		}
	}
	else {
		if (howToDone)
		{
			if (kougekiList == 0) {
				if (currentMousePos.x > nextLeftX && currentMousePos.y > nextTopY && currentMousePos.x < nextRightX && currentMousePos.y < nextBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						kougekiList = 1;
					}
				}
			}
			else if (kougekiList == 1) {
				if (titleEffect >= 1.0f) {
					tooHigh = true;
					tooLow = false;
				}
				else if (titleEffect <= 0.0f) {
					tooHigh = false;
					tooLow = true;
				}
				if (tooHigh) {
					titleEffect -= 0.01f;
				}
				else if (tooLow) {
					titleEffect += 0.01f;
				}
				if (currentMousePos.x > backLeftX && currentMousePos.y > backTopY && currentMousePos.x < backRightX && currentMousePos.y < backBotY) {
					if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
						kougekiList = 0;
					}
				}
				if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
					typeDone = true;
				}
				buttonSprite->Update(graphics.GetScreenWidth() / 2 - (buttonTexture->GetWidth() * 1.5f / 2), 600, buttonTexture->GetWidth() * 1.5f, buttonTexture->GetHeight() * 1.5f, 0, 0, buttonTexture->GetWidth(), buttonTexture->GetHeight(), 0, 1, 1, 1, titleEffect);
			}
			kougekiType->Update(0, 0, screenWidth, screenHeight, (screenWidth * kougekiList), 0, screenWidth, static_cast<float>(kougekiTypeTexture->GetHeight()), 0, 1, 1, 1, 1);
		}
		else
		{
			howToSprite->Update(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, 0, 1, 1, 1, 1);
			if (currentMousePos.x > howToLeft && currentMousePos.y > howToTop && currentMousePos.x < howToRight && currentMousePos.y < howToBot)
			{
				if (mouse.GetButtonDown() & Mouse::BTN_LEFT) {
					howToDone = true;
				}
			}
		}
	}
}

// 描画処理
void SceneGame::Render()
{
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	RenderContext rc;
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// 画面クリア＆レンダーターゲット設定
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// シャドウマップの描画
	shadowmapRenderer->Render(dc);

	// シーンの描画
	sceneRenderer->SetShadowmapData(shadowmapRenderer->GetShadowMapData());
	sceneRenderer->Render(dc);

	// ポストプロセス処理
	postprocessingRenderer->Render(dc);

	RenderEnemyGauge(dc, rc.view, rc.projection);
	// 2D関係の描画
	{
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
		RenderContext rc;
		rc.deviceContext = dc;
		shader->Begin(rc);
		if (typeDone){
			if (!playerUpdateStats) {
				if (difficultyChosen) {
					shader->Draw(rc, statsSprite.get());
					shader->Draw(rc, numberSprite.get());
					shader->Draw(rc, plusHealthSprite.get());
					shader->Draw(rc, plusDefSprite.get());
					shader->Draw(rc, plusAtkSprite.get());
					shader->Draw(rc, plusStaminaSprite.get());
				}
				else {
					shader->Draw(rc, difficultySprite.get());
				}
			}
			else {
				shader->Draw(rc, playerHealth.get());
				shader->Draw(rc, playerStamina.get());
				shader->Draw(rc, sceneGame.get());
				if (sceneUITimer > -1.0f){
					shader->Draw(rc, startUISprite.get());
				}
				if (boxer->GetPlayerKnockedState() == true || boxerX->GetEnemyKnockedState() == true){
					shader->Draw(rc, endUISprite.get());
				}
				if (boxer->GetPlayerKnockedState() == false && boxerX->GetEnemyKnockedState() == false){
					if (boxerX->GetStunedState() || boxerX->GetShieldBroke() != 0.0f) {
						shader->Draw(rc, enemyNotif.get());
					}
					if (boxer->GetStunedState() || boxer->GetShieldBroke() != 0.0f) {
						shader->Draw(rc, playerNotif.get());
					}
				}
			}
		}
		else{
			if (howToDone)
			{
				shader->Draw(rc, kougekiType.get());
				if (kougekiList == 1) {
					shader->Draw(rc, buttonSprite.get());
				}
			}
			else
			{
				shader->Draw(rc, howToSprite.get());
			}
		}
		shader->End(rc);
	}

	// デバッグ情報の表示
	{

	}
}

DirectX::XMFLOAT3 FactoringZXY(const DirectX::XMFLOAT4X4& rotation)
{
	DirectX::XMFLOAT3 rot;
	if( 1.0f - fabs( rotation.m[2][1] ) < 1.0e-6f )
	{
		rot.x	= rotation.m[2][1] < 0 ? DirectX::XM_PIDIV2 : -DirectX::XM_PIDIV2;
		rot.y	= atan2f( rotation.m[1][0], rotation.m[0][0] );
		rot.z	= 0;
	}
	else
	{
		rot.x	=  asinf( -rotation.m[2][1] );
		rot.y	= atan2f(  rotation.m[2][0], rotation.m[2][2] );
		rot.z	= atan2f(  rotation.m[0][1], rotation.m[1][1] );
	}
	return rot;
}

DirectX::XMFLOAT3 FactoringZXY(const DirectX::XMVECTOR& rotation)
{
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m,
		DirectX::XMMatrixRotationQuaternion(rotation));
	return FactoringZXY(m);
}

DirectX::XMFLOAT3 FactoringZXY(const DirectX::XMFLOAT4& rotation)
{
	return FactoringZXY(DirectX::XMLoadFloat4(&rotation));
}

void SceneGame::DrawDebugParameter(Model* model, DirectX::XMFLOAT4X4& transform, const char* label)
{
	int meshCount = 0;
	ImGui::PushID(label);
	if (ImGui::TreeNode(label))
	{
		DirectX::XMVECTOR Scale, Rotation, Position;
		DirectX::XMMatrixDecompose(&Scale, &Rotation, &Position, DirectX::XMLoadFloat4x4(&transform));
		DirectX::XMFLOAT3 scale, rotation, position;
		DirectX::XMStoreFloat3(&scale, Scale);
		rotation = FactoringZXY(Rotation);
		DirectX::XMStoreFloat3(&position, Position);
		ImGui::SliderFloat3("scale", &scale.x, 0.0f, 10.0f);
		ImGui::SliderFloat3("rotation", &rotation.x, -DirectX::XM_PI, DirectX::XM_PI);
		ImGui::SliderFloat3("position", &position.x, -200.0f, 200.0f);
		DirectX::XMMATRIX Transform;
		Transform	= DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)
					* DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z)
					* DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		DirectX::XMStoreFloat4x4(&transform, Transform);
		// モデルのデバッグ情報表示
		model->DrawDebugGUI();
		ImGui::TreePop();
	}
	ImGui::PopID();
}


void SceneGame::RenderEnemyGauge(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	Graphics& graphics = Graphics::Instance();
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y += enemy->GetHeight();
		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);
		DirectX::XMVECTOR ScreenPosition =DirectX::XMVector3Project(WorldPosition,viewport.TopLeftX, viewport.TopLeftY,viewport.Width, viewport.Height,viewport.MinDepth, viewport.MaxDepth,Projection, View, World);

		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);
		const float gaugeWidth = 120.0f;
		const float gaugeHeight = 8.0f;
		float healthRate = static_cast<float>(enemy->GetHealth()) / static_cast<float>(enemy->GetMaxHealth());
		float stamRate = static_cast<float>(enemy->GetStamina()) / static_cast<float>(enemy->GetMaxStamina());
		if (healthRate < 0){
			healthRate = 0.0f;
		}
		if (stamRate < 0){
			stamRate = 0.0f;
		}

		gauge->Update(screenPosition.x - 60.0f, screenPosition.y - 20.0f, gaugeWidth * healthRate, gaugeHeight,
			0, 0,
			static_cast<float>(gauge->GetTextureWidth()),
			static_cast<float>(gauge->GetTextureHeight()),
			0.0f,
			1.0f, 0.0f, 0.0f, 1.0f
		);
		baseGauge->Update(screenPosition.x - 60.0f, screenPosition.y - 20.0f, gaugeWidth, gaugeHeight,
			0, 0,
			static_cast<float>(baseGauge->GetTextureWidth()),
			static_cast<float>(baseGauge->GetTextureHeight()),
			0.0f,
			1.0f, 0.0f, 0.0f, 1.0f
		);
		stamGauge->Update(screenPosition.x - 60.0f, screenPosition.y - 10.0f, gaugeWidth * stamRate, gaugeHeight,
			0, 0,
			static_cast<float>(stamGauge->GetTextureWidth()),
			static_cast<float>(stamGauge->GetTextureHeight()),
			0.0f,
			0.0f, 0.0f, 1.0f, 1.0f
		);
		SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
		RenderContext rc;
		rc.deviceContext = dc;
		shader->Begin(rc);
		shader->Draw(rc, baseGauge.get());
		shader->Draw(rc, gauge.get());
		shader->Draw(rc, stamGauge.get());
		shader->End(rc);
	}
}