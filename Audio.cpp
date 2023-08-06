#include "misc.h"
#include "Audio.h"

//Audio* Audio::instance = nullptr;

// コンストラクタ
Audio::Audio()
{
	HRESULT hr;

	// COMの初期化
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	UINT32 createFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	//createFlags |= XAUDIO2_DEBUG_ENGINE;
#endif

	// XAudio初期化
	hr = XAudio2Create(&xaudio, createFlags);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// マスタリングボイス生成
	hr = xaudio->CreateMasteringVoice(&masteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

// デストラクタ
Audio::~Audio()
{
	// マスタリングボイス破棄
	if (masteringVoice != nullptr)
	{
		masteringVoice->DestroyVoice();
		masteringVoice = nullptr;
	}

	// XAudio終了化
	if (xaudio != nullptr)
	{
		xaudio->Release();
		xaudio = nullptr;
	}

	// COM終了化
	CoUninitialize();
}

// オーディオソース読み込み
std::shared_ptr<AudioSource> Audio::LoadAudioSource(const char* filename)
{
	// 指定されたファイル名のモデルが既に読み込まれているかどうかをチェック
	auto it = audioSources.find(filename);
	if (it != audioSources.end())
	{
		// 読み込まれている場合はそのまま返す
		return it->second;
	}


	// 読み込まれていない場合は新規に読み込む
	std::shared_ptr<AudioResource> resource = std::make_shared<AudioResource>(filename);
	auto source = std::make_shared<AudioSource>(xaudio, resource);
	audioSources[filename] = source;
	return source;
}
