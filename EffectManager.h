#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include "render.h"

class EffectManager
{
private:
	EffectManager(){}
	~EffectManager(){}

public:
	// �C���X�^���X�̎擾
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	// ������
	void Init(render* R);
	// �X�V
	void Update(float elapsedTime);
	// �`��
	void Draw(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projecttion);
	// �I��
	void Uninit();

	// Effekseer Manager�̎擾
	Effekseer::ManagerRef GetManager() { return effekseerManager; }

private:
	Effekseer::ManagerRef			effekseerManager;
	EffekseerRenderer::RendererRef	effekseerRenderer;
	render* Render;
};