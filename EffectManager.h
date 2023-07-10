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
	// インスタンスの取得
	static EffectManager& Instance()
	{
		static EffectManager instance;
		return instance;
	}

	// 初期化
	void Init(render* R);
	// 更新
	void Update(float elapsedTime);
	// 描画
	void Draw(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projecttion);
	// 終了
	void Uninit();

	// Effekseer Managerの取得
	Effekseer::ManagerRef GetManager() { return effekseerManager; }

private:
	Effekseer::ManagerRef			effekseerManager;
	EffekseerRenderer::RendererRef	effekseerRenderer;
	render* Render;
};