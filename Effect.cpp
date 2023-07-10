#include "Effect.h"
#include "EffectManager.h"
#include "render.h"

Effect::Effect(const char* filename, render* Render)
{
	this->Render = Render;
	std::lock_guard<std::mutex> lock(this->Render->get_mutex());

	// effekseerはutf-16しか対応しないので、ファイル名をUTF-16に変換
	char16_t utf16filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16filename, 256, filename);

	// Managerを取得
	Effekseer::ManagerRef manager = EffectManager::Instance().GetManager();

	// エフェクトを読み込む
	effekseerEffect = Effekseer::Effect::Create(manager, (EFK_CHAR*)utf16filename);
}

Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	// Managerを取得
	auto manager = EffectManager::Instance().GetManager();

	// エフェクトを再生
	auto handle = manager->Play(effekseerEffect, position.x, position.y, position.z);

	// スケールを設定
	manager->SetScale(handle, scale, scale, scale);

	return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
	// Managerを取得
	auto manager = EffectManager::Instance().GetManager();

	// エフェクトを停止
	manager->StopEffect(handle);
}

void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	// Managerを取得
	auto manager = EffectManager::Instance().GetManager();

	// エフェクトの位置を設定
	manager->SetLocation(handle, position.x, position.y, position.z);
}

void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	// Managerを取得
	auto manager = EffectManager::Instance().GetManager();

	// エフェクトのスケールを設定
	manager->SetScale(handle, scale.x, scale.y, scale.z);
}
