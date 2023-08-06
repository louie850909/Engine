#include "misc.h"
#include "Audio.h"

//Audio* Audio::instance = nullptr;

// �R���X�g���N�^
Audio::Audio()
{
	HRESULT hr;

	// COM�̏�����
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio������
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �}�X�^�����O�{�C�X����
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// �f�X�g���N�^
Audio::~Audio()
{
	// �}�X�^�����O�{�C�X�j��
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio�I����
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM�I����
	CoUninitialize();
}

// �I�[�f�B�I�\�[�X�ǂݍ���
std::shared_ptr<AudioSource> Audio::LoadAudioSource(const char* filename)
{
	// �w�肳�ꂽ�t�@�C�����̃��f�������ɓǂݍ��܂�Ă��邩�ǂ������`�F�b�N
	auto it = audioSources.find(filename);
	if (it != audioSources.end())
	{
		// �ǂݍ��܂�Ă���ꍇ�͂��̂܂ܕԂ�
		return it->second;
	}


	// �ǂݍ��܂�Ă��Ȃ��ꍇ�͐V�K�ɓǂݍ���
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	auto source = std::make_shared<AudioSource>(xaudio, resource);
	audioSources[filename] = source;
	return source;
}
