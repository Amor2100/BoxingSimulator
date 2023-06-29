#pragma once

#include "Renderer/ModelRenderer.h"

// �V�[�������_���[
class SceneRenderer : public ModelRenderer
{
public:
	SceneRenderer(UINT width, UINT height);
	~SceneRenderer(){}

	// �V���h�E�}�b�v����RenderContext�ɐς�
	void SetShadowmapData(ShadowMapData shadowMapData) { this->shadowMapData = shadowMapData; }

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
	std::unique_ptr<RenderTarget>	renderTarget;		//	�I�t�X�N���[�������_�����O�p�`��o�b�t�@
	std::unique_ptr<DepthStencil>	depthStencil;		//	�I�t�X�N���[�������_�����O�p�[�x�o�b�t�@
	std::unique_ptr<Sprite>			skyboxRenderer;		//	��`��p�X�v���C�g
	ShadowMapData					shadowMapData;		//	�V���h�E�}�b�v���
	bool							drawDebugPrimitive;	//	�f�o�b�O�v���~�e�B�u�`��
	int timer;
};