#pragma once
#include <Windows.h>
#include <xaudio2.h>

class Sound
{
public:
	enum SOUND_LABEL
	{
		SOUND_LABEL_BGM_Title,		// Title BGM
		SOUND_LABEL_BGM_Tutorial,	// Tutorial BGM
		SOUND_LABEL_BGM_Game,		// Game BGM
		SOUND_LABEL_BGM_End,		// End BGM
		SOUND_LABEL_SE_walk,		// 歩きSE
		SOUND_LABEL_SE_defend001,	// 
		SOUND_LABEL_SE_hit000,		// 
		SOUND_LABEL_SE_laser000,	// 
		SOUND_LABEL_SE_lockon000,	// 
		SOUND_LABEL_SE_shot000,		// 
		SOUND_LABEL_SE_shot001,		// 
		SOUND_LABEL_MAX,
	};

private:
	typedef struct
	{
		char* pFilename;	// ファイル名
		int nCntLoop;		// ループカウント
	} SOUNDPARAM;

	// 各音素材のパラメータ
	SOUNDPARAM Param[SOUND_LABEL_MAX] =
	{
		{ (char*)"resources/BGM/BGM_Title.wav", -1 },	// Title BGM
		{ (char*)"resources/BGM/BGM_Tutorial.wav", -1 },	// Tutorial BGM
		{ (char*)"resources/BGM/BGM_Game.wav", -1 },		// Game BGM
		{ (char*)"resources/BGM/BGM_End.wav", -1 },		// End BGM
		{ (char*)"resources/SE/walk.wav", 0 },			// 歩き音
		{ (char*)"resources/SE/defend000.wav", 0 },		// 弾発射音
		{ (char*)"resources/SE/defend001.wav", 0 },		// 弾発射音
		{ (char*)"resources/SE/hit000.wav", 0 },			// 弾発射音
		{ (char*)"resources/SE/laser000.wav", 0 },		// 弾発射音
		{ (char*)"resources/SE/lockon000.wav", 0 },		// 弾発射音
		{ (char*)"resources/SE/shot000.wav", 0 },		// 弾発射音
	};

	Sound(){}
	~Sound(){}

public:
	static Sound& getInstance()
	{
		static Sound instance;
		return instance;
	}

	void InitSound(HWND hWnd);
	void UninitSound(void);
	void Play(int label);
	void Stop(int label);
	void Stop(void);

private:
	IXAudio2* XAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice* MasteringVoice = NULL;			// マスターボイス
	IXAudio2SourceVoice* SourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
	BYTE* DataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD SizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset);
};
