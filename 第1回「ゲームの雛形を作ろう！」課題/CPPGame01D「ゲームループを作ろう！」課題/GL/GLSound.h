#ifndef _GLSOUND_H_
#define	_GLSOUND_H_
//******************************************************************************
//
//
//		Windows関連処理
//
//
//******************************************************************************

namespace GLSound
{
	void Init();
	void Uninit();
	void Update();

	// サウンド制御
	void StopSound();
	void PauseSound();
	void ResumeSound();
	// Wave
	//void ResizeWave(int n);
	void LoadWave(LOAD_WAVE* pLoad);
	void ReleaseWave();
	// SE
	void ResizeSE(int n);
	void PlaySE(WAVE_DATA& wave);
	void StopSE();
	// BGM
	void PlayBGM(WAVE_DATA& wave, bool loop = true);
	void StopBGM();
	void FadeoutBGM(int fadeTime);
	void TempoupBGM();
	// 音声
	void PlayVoice(WAVE_DATA& wave);
	void StopVoice();
};

//******************************************************************************
#endif // !_GLSOUND_H_