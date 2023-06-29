#include "Graphics/Graphics.h"
#include "SceneTitle.h"
#include "SceneWin.h"
#include "SceneManager.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"


void SceneWin::Initialize()
{
	Graphics& graphics = Graphics::Instance();
	// 2D‰æ‘œ‚ð•\Ž¦‚·‚é
	{
		winTexture = std::make_unique<Texture>("Data/Texture/scenewinyellow.png");
		winSprite = std::make_unique<Sprite>();
		winSprite->SetShaderResourceView(winTexture->GetShaderResourceView(), winTexture->GetWidth(), winTexture->GetHeight());
		winSprite->Update(0, 0, graphics.GetScreenWidth(), graphics.GetScreenHeight(),
			0, 0, static_cast<float>(winTexture->GetWidth()), static_cast<float>(winTexture->GetHeight()),
			0,
			1, 1, 1, 1);
	}
}

void SceneWin::Finalize()
{
}

void SceneWin::Update(float elapsedTime)
{
	Graphics& graphics = Graphics::Instance();
	SetCursorPos(graphics.GetScreenWidth() / 2, graphics.GetScreenHeight() / 2);
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		// Close the program
		PostQuitMessage(0);
	}
}

void SceneWin::Render()
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

		// 2DŠÖŒW‚Ì•`‰æ
		{
			SpriteShader* shader = graphics.GetShader(SpriteShaderId::Default);
			RenderContext rc;
			rc.deviceContext = dc;
			shader->Begin(rc);
			shader->Draw(rc, winSprite.get());
			shader->End(rc);
		}
	}
}