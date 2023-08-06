#pragma once

#include <xaudio2.h>
#include "AudioSource.h"
#include <map>
#include <string>

// �I�[�f�B�I
class Audio
{
public:
	

public:
	// �C���X�^���X�擾
	static Audio& Instance()
	{ 
		static Audio instance;
		return instance;
	}

	// �I�[�f�B�I�\�[�X�ǂݍ���
	std::shared_ptr<AudioSource> LoadAudioSource(const char* filename);

private:
	Audio();
	~Audio();

	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;

	using AudioSourceMap = std::map<std::string, std::shared_ptr<AudioSource>>;
	AudioSourceMap audioSources;
};
