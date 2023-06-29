#pragma once

#include "Renderer/ModelRenderer.h"

// �V���h�E�}�b�v�����_���[
class ShadowmapRenderer : public ModelRenderer
{
	static constexpr float Near = 1.0f;
	static constexpr float Far = 1000.0f;
public:
	ShadowmapRenderer(UINT shadowmapSize);
	~ShadowmapRenderer();

	// �V���h�E�}�b�v����RenderContext�ɐς�
	ShadowMapData GetShadowMapData();

public:
	//	�`����s
	void Render(ID3D11DeviceContext* dc) override;

	//	�f�o�b�OGUI�`��
	void DrawDebugGUI() override;

	//	RenderTarget��ShaderResourceView���擾
	const SRVHandleList GetRenderTargetShaderResourceViews() const override;

	//	DepthStencil��ShaderResourceView���擾
	const SRVHandle GetDepthStencilShaderResourceView() const override;

private:
	//	�V���h�E�}�b�v�Ŏg�p����e�s����Z�o
	void CalcShadowmapMatrix(DirectX::XMFLOAT4X4& view, DirectX::XMFLOAT4X4& projection);

private:
	std::unique_ptr<DepthStencil>	depthStencil;						// �V���h�E�}�b�v�p�[�x�o�b�t�@
	float							shadowRect	= 10.0f;				// �`��͈�
	float							shadowBias	= 0.0001f;				// �[�x��r�p�̃I�t�Z�b�g�l
	DirectX::XMFLOAT3				shadowColor	= {0.0f, 0.0f, 0.0f};	// �e�̐F
};

