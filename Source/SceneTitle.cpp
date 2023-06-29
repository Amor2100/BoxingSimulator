#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneGame.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "SceneLoading.h"
#include "Graphics/Graphics.h"


void SceneTitle::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	// 2D‰æ‘œ‚ð•\Ž¦‚·‚é
	{
		title = std::make_unique<Texture>("Data/Texture/titlescene.png");
		buttonTexture = std::make_unique<Texture>("Data/Texture/continuebutton.png");
		loadingTexture = std::make_unique<Texture>("Data/Texture/loadingui.png");
		sprite = std::make_unique<Sprite>();
		buttonSprite = std::make_unique<Sprite>();
		loadingSprite = std::make_unique<Sprite>();
		sprite->SetShaderResourceView(title->GetShaderResourceView(), title->GetWidth(), title->GetHeight());
		buttonSprite->SetShaderResourceView(buttonTexture->GetShaderResourceView(), buttonTexture->GetWidth(), buttonTexture->GetHeight());
		loadingSprite->SetShaderResourceView(loadingTexture->GetShaderResourceView(), loadingTexture->GetWidth(), loadingTexture->GetHeight());
		sprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>(title->GetWidth()), static_cast<float>(title->GetHeight()),
			0,
			1, 1, 1, 1);
		//buttonSprite->Update(graphics.GetScreenWidth() / 2 - (buttonTexture->GetWidth() * 1.5f / 2), 600, buttonTexture->GetWidth() * 1.5f, buttonTexture->GetHeight() * 1.5f, 0, 0, buttonTexture->GetWidth(), buttonTexture->GetHeight(), 0, 1, 1, 1, 0);
		loadingSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(), 0, 0, loadingTexture->GetWidth(), loadingTexture->GetHeight(), 0, 1, 1, 1, 1);
	}
}

void SceneTitle::Finalize()
{
}

void SceneTitle::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	SetCursorPos(graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2);
	buttonSprite->Update(graphics.GetScreenWidth() / 2 - (buttonTexture->GetWidth() * 1.5f / 2), 600, buttonTexture->GetWidth() * 1.5f, buttonTexture->GetHeight() * 1.5f, 0, 0, buttonTexture->GetWidth(), buttonTexture->GetHeight(), 0, 1, 1, 1, titleEffect);
	if (titleEffect >= 1.0f)
	{
		tooHigh = true;
		tooLow = false;
	}
	else if (titleEffect <= 0.0f)
	{
		tooHigh = false;
		tooLow = true;
	}
	if (tooHigh)
	{
		titleEffect -= 0.01f;
	}
	else if (tooLow)
	{
		titleEffect += 0.01f;
	}
	
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		Loading = true;
		SceneManager::Instance().ChangeScene(new SceneGame);
	}
}

void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	{
		float screenWidth = static_cast<float>(graphics.GetScreenWidth());
		float screenHeight = static_cast<float>(graphics.GetScreenHeight());
		float textureWidth = static_cast<float>(sprite->GetTextureWidth());
		float textureHeight = static_cast<float>(sprite->GetTextureHeight());

		// 2DŠÖŒW‚Ì•`‰æ
		{
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
			RenderContext rc;
			rc.deviceContext = dc;
			shader->Begin(rc);
			shader->Draw(rc, sprite.get());
			shader->Draw(rc, buttonSprite.get());
			if (Loading)
			{
				shader->Draw(rc, loadingSprite.get());
			}
			shader->End(rc);
		}
	}
}