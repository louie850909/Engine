#pragma once

#include <xaudio2.h>
#include "AudioSource.h"
#include <map>
#include <string>

// オーディオ
class Audio
{
public:
	

public:
	// インスタンス取得
	static Audio& Instance()
	{ 
		static Audio instance;
		return instance;
	}

	// オーディオソース読み込み
	std::unique_ptr<AudioSource> LoadAudioSource(const char* filename);

private:
	Audio();
	~Audio();

	IXAudio2*				xaudio = nullptr;
	IXAudio2MasteringVoice* masteringVoice = nullptr;
};
