#pragma once
#include <xaudio2.h>
#include <string>

struct WaveData {
	WAVEFORMATEX waveFormat;
	char* soundBuffer;
	DWORD size;

	~WaveData() { free(soundBuffer); }
};

namespace SoundManager {

	inline IXAudio2* xaudio = nullptr;
	inline IXAudio2MasteringVoice* masteringVoice = nullptr;
	inline IXAudio2SourceVoice* sourceVoice = nullptr;
	inline XAUDIO2_VOICE_STATE state = {};

	HRESULT initialize();
	bool LoadFile(const std::wstring path, WaveData* outData);
	bool Play(const std::wstring& fileName, WaveData* data, bool loop);
	void Stop();
	void Release();

}