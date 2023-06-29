#pragma once

#include <memory>
#include <wrl.h>
#include "Graphics/Shader.h"

class FinalpassShader : public SpriteShader
{
public:
	FinalpassShader(ID3D11Device* device);
	~FinalpassShader() override {}

	void Begin(const RenderContext& rc) override;
	void Draw(const RenderContext& rc, const Sprite* sprite) override;
	void End(const RenderContext& rc) override;

private:
	struct CBFinalpass
	{
		DirectX::XMFLOAT4	screenSize;	// xy : サイズ, z : nearクリップ, w : farクリップ
		//	カメラ情報
		DirectX::XMFLOAT4	viewPosition;
		DirectX::XMFLOAT4X4	view;
		DirectX::XMFLOAT4X4	projection;
		DirectX::XMFLOAT4X4	inverseView;
		DirectX::XMFLOAT4X4	inverseProjection;
		//	色調補正
		float				hueShift;	// 色相調整
		float				saturation;	// 彩度調整
		float				brightness;	// 明度調整
		float				colorGradiationDummy;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer>				finalpassConstantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			shadowMapSamplerState;
};
