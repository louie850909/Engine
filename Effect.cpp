#include "Effect.h"
#include "EffectManager.h"
#include "render.h"

Effect::Effect(const char* filename, render* Render)
{
	this->Render = Render;
	std::lock_guard<std::mutex> lock(this->Render->get_mutex());

	// effekseer��utf-16�����Ή����Ȃ��̂ŁA�t�@�C������UTF-16�ɕϊ�
	char16_t utf16filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16filename, 256, filename);

	// Manager���擾
	Effekseer::ManagerRef manager = EffectManager::Instance().GetManager();

	// �G�t�F�N�g��ǂݍ���
	effekseerEffect = Effekseer::Effect::Create(manager, (EFK_CHAR*)utf16filename);
}

Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
	// Manager���擾
	auto manager = EffectManager::Instance().GetManager();

	// �G�t�F�N�g���Đ�
	auto handle = manager->Play(effekseerEffect, position.x, position.y, position.z);

	// �X�P�[����ݒ�
	manager->SetScale(handle, scale, scale, scale);

	return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
	// Manager���擾
	auto manager = EffectManager::Instance().GetManager();

	// �G�t�F�N�g���~
	manager->StopEffect(handle);
}

void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	// Manager���擾
	auto manager = EffectManager::Instance().GetManager();

	// �G�t�F�N�g�̈ʒu��ݒ�
	manager->SetLocation(handle, position.x, position.y, position.z);
}

void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	// Manager���擾
	auto manager = EffectManager::Instance().GetManager();

	// �G�t�F�N�g�̃X�P�[����ݒ�
	manager->SetScale(handle, scale.x, scale.y, scale.z);
}
