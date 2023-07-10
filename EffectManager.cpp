#include "EffectManager.h"

void EffectManager::Init(render* R)
{
	Render = R;

	// �����_���̐���
	effekseerRenderer = ::EffekseerRendererDX11::Renderer::Create(Render->get_device(), Render->get_immediate_context(), 2048);

	// �G�t�F�N�g�}�l�[�W���̐���
	effekseerManager = ::Effekseer::Manager::Create(2048);

	// �����_���̐ݒ�
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// ���[�_�[�̐ݒ�
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	// ������W�n�̐ݒ�
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

void EffectManager::Update(float elapsedTime)
{
	effekseerManager->Update(elapsedTime * 60.0f);
}

void EffectManager::Draw(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projecttion)
{
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*> (&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*> (&projecttion));
	
	effekseerRenderer->BeginRendering();
	effekseerManager->Draw();
	effekseerRenderer->EndRendering();
}

void EffectManager::Uninit()
{
}
