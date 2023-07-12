#include "sound.h"
#include "misc.h"
void Sound::InitSound(HWND hWnd)
{
	HRESULT hr{ S_OK };

	// COM���C�u�����̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&XAudio2, 0);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �}�X�^�[�{�C�X�̐���
	hr = XAudio2->CreateMasteringVoice(&MasteringVoice);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// �T�E���h�f�[�^�̏�����
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// �o�b�t�@�̃N���A
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// �T�E���h�f�[�^�t�@�C���̐���
		hFile = CreateFile((LPCWSTR)Param[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		_ASSERT_EXPR(hFile != INVALID_HANDLE_VALUE, hr_trace(hr));

		// WAVE�t�@�C���̃`�F�b�N
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		_ASSERT_EXPR(dwFiletype == 'EVAW', "Not WAVE file");

		// �t�H�[�}�b�g�`�F�b�N
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �I�[�f�B�I�f�[�^�ǂݍ���
		hr = CheckChunk(hFile, 'atad', &SizeAudio[nCntSound], &dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		DataAudio[nCntSound] = (BYTE*)malloc(SizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, DataAudio[nCntSound], SizeAudio[nCntSound], dwChunkPosition);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �\�[�X�{�C�X�̐���
		hr = XAudio2->CreateSourceVoice(&SourceVoice[nCntSound], &(wfx.Format));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// �o�b�t�@�̒l�ݒ�
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = SizeAudio[nCntSound];
		buffer.pAudioData = DataAudio[nCntSound];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount = Param[nCntSound].nCntLoop;

		// �I�[�f�B�I�o�b�t�@�̓o�^
		SourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	}
}

void Sound::UninitSound(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (SourceVoice[nCntSound])
		{
			// �ꎞ��~
			SourceVoice[nCntSound]->Stop(0);

			// �\�[�X�{�C�X�̔j��
			SourceVoice[nCntSound]->DestroyVoice();
			SourceVoice[nCntSound] = NULL;

			// �I�[�f�B�I�f�[�^�̊J��
			free(DataAudio[nCntSound]);
			DataAudio[nCntSound] = NULL;
		}
	}

	// �}�X�^�[�{�C�X�̔j��
	MasteringVoice->DestroyVoice();
	MasteringVoice = NULL;

	if (XAudio2)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		XAudio2->Release();
		XAudio2 = NULL;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}

void Sound::Play(int label)
{
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = SizeAudio[label];
	buffer.pAudioData = DataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = Param[label].nCntLoop;

	// ��Ԏ擾
	SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		SourceVoice[label]->FlushSourceBuffers();
	}

	// �I�[�f�B�I�o�b�t�@�̓o�^
	SourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	SourceVoice[label]->Start(0);
}

void Sound::Stop(int label)
{
	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	SourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		SourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		SourceVoice[label]->FlushSourceBuffers();
	}
}

void Sound::Stop(void)
{
	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (SourceVoice[nCntSound])
		{
			// �ꎞ��~
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
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
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
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, buffer, buffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}
