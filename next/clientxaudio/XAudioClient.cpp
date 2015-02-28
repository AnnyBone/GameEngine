#include "stdafx.h"

#include <xaudio2.h>

IXAudio2	*aXAudioInstance = NULL;

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT		hResult;

	// Create the instance.
	hResult = XAudio2Create(&aXAudioInstance, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hResult))
		return hResult;

	IXAudio2MasteringVoice	*mvMaster = NULL;

	// Create mastering voice.
	hResult = aXAudioInstance->CreateMasteringVoice(&mvMaster);
	if (FAILED(hResult))
		return hResult;

	return 0;
}

