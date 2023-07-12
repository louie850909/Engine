#include "sound.h"
#include "misc.h"
void Sound::InitSound(HWND hWnd)
{
	HRESULT hr{ S_OK };

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&XAudio2, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// マスターボイスの生成
	hr = XAudio2->CreateMasteringVoice(&MasteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// サウンドデータの初期化
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile((LPCWSTR)Param[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		_ASSERT_EXPR(hFile != INVALID_HANDLE_VALUE, hr_trace(hr));

		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		_ASSERT_EXPR(dwFiletype == 'EVAW', "Not WAVE file");

		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &SizeAudio[nCntSound], &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		DataAudio[nCntSound] = (BYTE*)malloc(SizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, DataAudio[nCntSound], SizeAudio[nCntSound], dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// ソースボイスの生成
		hr = XAudio2->CreateSourceVoice(&SourceVoice[nCntSound], &(wfx.Format));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = SizeAudio[nCntSound];
		buffer.pAudioData = DataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = Param[nCntSound].nCntLoop;

		// オーディオバッファの登録
		SourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}
}

void Sound::UninitSound(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (SourceVoice[nCntSound])
		{
			// 一時停止
			SourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			SourceVoice[nCntSound]->DestroyVoice();
			SourceVoice[nCntSound] = NULL;

			// オーディオデータの開放
			free(DataAudio[nCntSound]);
			DataAudio[nCntSound] = NULL;
		}
	}

	// マスターボイスの破棄
	MasteringVoice->DestroyVoice();
	MasteringVoice = NULL;

	if (XAudio2)
	{
		// XAudio2オブジェクトの開放
		XAudio2->Release();
		XAudio2 = NULL;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

void Sound::Play(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = SizeAudio[label];
	buffer.pAudioData = DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = Param[label].nCntLoop;

	// 状態取得
	SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		SourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	SourceVoice[label]->Start(0);
}

void Sound::Stop(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		SourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		SourceVoice[label]->FlushSourceBuffers();
	}
}

void Sound::Stop(void)
{
	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (SourceVoice[nCntSound])
		{
			// 一時停止
			SourceVoice[nCntSound]->Stop(0);
		}
	}
}

HRESULT Sound::CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

HRESULT Sound::ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
{
	DWORD dwRead;

	if (SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
