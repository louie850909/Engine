#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include "render.h"

class Effect
{
public:
	Effect(const char* filename, render* Render);
	~Effect(){}

	Effekseer::Handle Play(const DirectX::XMFLOAT3& position, float scale = 1.0f);
	void Stop(Effekseer::Handle handle);
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

private:
	Effekseer::EffectRef effekseerEffect;
	render* Render;
};