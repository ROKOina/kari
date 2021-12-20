//******************************************************************************
//
//
//		サウンド関連処理
//
//
//******************************************************************************

//------< include >-------------------------------------------------------------

#include "GLAll.h"
#include <xaudio2.h>
#include <vector>
using namespace std;

//------< extern >--------------------------------------------------------------
//------< define >--------------------------------------------------------------

#define SE_MAX			0x100				// SEの同時再生可能数
#define QUEUE_SIZE		0x100				// 音声キューのサイズ

#define TEMPOUP_RATE	1.1f				// テンポアップ率

//------< structure >-----------------------------------------------------------

//--------------------------------
// XA_Voiceクラス
//--------------------------------
class XA_Voice
{
protected:
	IXAudio2SourceVoice*	m_pVoice;

public:
	XA_Voice();
	virtual ~XA_Voice();

	//virtual void Play(GLWave *pWave, bool loop = false);
	virtual void Play(WAVE_DATA* pWave, bool loop = false);
	virtual void Stop();

	void Pause();
	void Resume();

	float GetVolume();
	void SetVolume(float volume);

	float GetFrequency();
	void SetFrequency(float ratio);

	bool isPlaying();
};

//------< data >----------------------------------------------------------------
//------< work >----------------------------------------------------------------

IXAudio2*				m_pXAudio2;
static IXAudio2MasteringVoice*	m_pMasteringVoice;

static vector<XA_Voice>			m_se;

static XA_Voice					m_bgm;
static int						m_fadeTime;					// フェードアウト時間（0：フェードアウトなし）
static bool						m_pause;

static XA_Voice					m_voice;					// 音声用ボイス
//static int						m_voice_queue[QUEUE_SIZE];	// サウンドキュー
static WAVE_DATA*				m_voice_queue[QUEUE_SIZE];	// サウンドキュー
static int						m_voice_read;				// 読み出しポインタ
static int						m_voice_write;				// 書き込みポインタ

static WAVE_DATA*				m_prevSE;					// 直前に再生されたSE
		
//------< prototype >-----------------------------------------------------------
//==============================================================================
//
//		サウンドシステム
//
//==============================================================================

// 初期設定
void GLSound::Init()
{
	// 変数初期化
	m_fadeTime = 0;
	m_pause = false;

	m_voice_read = 0;
	m_voice_write = 0;

	m_prevSE = nullptr;

	ResizeSE(SE_MAX);

	// Waveの設定
	pGLWaveManager->Init();

	// XAudio2の初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	XAudio2Create(&m_pXAudio2, 0);
	m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
}

//==============================================================================

// 終了処理
void GLSound::Uninit()
{
	// ボイスの停止
	StopSound();

	// XAudio2の終了処理
	if (m_pMasteringVoice) m_pMasteringVoice->DestroyVoice();
	if (m_pXAudio2) m_pXAudio2->Release();
	CoUninitialize();

	// Waveの解放
	pGLWaveManager->Uninit();
}

//==============================================================================

// サウンド更新処理
void GLSound::Update()
{
	// サウンドキュー処理
	if (m_voice_read != m_voice_write) {						// キューにデータが残っているとき
		if (!m_voice.isPlaying()) {
			//WAVE_DATA *pWave = pGLWaveManager->getResource(m_voice_queue[m_voice_read]);
			WAVE_DATA *pWave = m_voice_queue[m_voice_read];
			m_voice.Play(pWave);
			m_voice_read = (m_voice_read + 1) % QUEUE_SIZE;		// リードポインタを進める
		}
	}

	if (!m_pause) {
		// フェードアウトチェック
		if (m_fadeTime) {
			m_bgm.SetVolume(m_bgm.GetVolume() - 1.0f / m_fadeTime);
			if (m_bgm.GetVolume() <= 0.0f) GLSound::StopBGM();
		}
	}

	m_prevSE = nullptr;
}

//==============================================================================
//
//		サウンド制御
//
//==============================================================================

// 全サウンド停止
void GLSound::StopSound()
{
	StopSE();
	StopBGM();
	StopVoice();
}

// ポーズ
void GLSound::PauseSound()
{
	StopSE();
	StopVoice();
	m_bgm.Pause();
	m_pause = true;
}

// ポーズ解除
void GLSound::ResumeSound()
{
	m_bgm.Resume();
	m_pause = false;
}

//==============================================================================
//
//		Wave
//
//==============================================================================

// Wave数の変更
//void GLSound::ResizeWave(int n)
//{
//	pGLWaveManager->Resize(n);
//}

// Waveのロード
void GLSound::LoadWave(LOAD_WAVE* pLoad)
{
	pGLWaveManager->Load(pLoad);
}

// Waveの解放
void GLSound::ReleaseWave()
{
	pGLWaveManager->ReleaseAll();
}

//==============================================================================
//
//		SE
//
//==============================================================================

// SE数の変更
void GLSound::ResizeSE(int n)
{
	StopSE();
	m_se.resize(n);
}

// SEの再生
void GLSound::PlaySE(WAVE_DATA& wave)
{
	//GLWave* pWave = pGLWaveManager->getResource(wave);
	//if (!pWave) return;

	if (m_prevSE == &wave) return;		// 直前と同じSEのときは再生しない

	for (auto& se : m_se) {
		if (!se.isPlaying()) {
			se.Play(&wave, false);
			m_prevSE = &wave;
			break;
		}
	}
}

// SEの停止
void GLSound::StopSE()
{
	for (auto& se : m_se) se.Stop();
}

//==============================================================================
//
//		BGM
//
//==============================================================================

// BGMの再生
void GLSound::PlayBGM(WAVE_DATA& wave, bool loop)
{
	StopBGM();

	//GLWave	*pWave = pGLWaveManager->getResource(wave);
	m_bgm.Play(&wave, loop);
}

// BGMの停止
void GLSound::StopBGM()
{
	m_bgm.Stop();
	m_fadeTime = 0;
	m_pause = false;
}

// フェードアウト
void GLSound::FadeoutBGM(int fadeTime)
{
	m_fadeTime = fadeTime;
}

// テンポアップ
void GLSound::TempoupBGM()
{
	m_bgm.SetFrequency(m_bgm.GetFrequency() * TEMPOUP_RATE);
}

//==============================================================================
//
//		音声
//
//==============================================================================

// 音声の再生（キューへのPUSH）
void GLSound::PlayVoice(WAVE_DATA& wave)
{
	int	next = (m_voice_write + 1) % QUEUE_SIZE;
	if (next == m_voice_read) return;

	m_voice_queue[m_voice_write] = &wave;			// キューへWaveを保存
	m_voice_write = next;							// ライトポインタを進める
}

// 音声の停止（キューのFLUSH）
void GLSound::StopVoice()
{
	m_voice.Stop();
	m_voice_read = m_voice_write;					// キューの停止
}

//==============================================================================
//
//		XA_Voiceクラス
//
//==============================================================================

// コンストラクタ
XA_Voice::XA_Voice() : m_pVoice(0)
{
}

// デストラクタ
XA_Voice::~XA_Voice()
{
	Stop();
}

//==============================================================================

// 再生
void XA_Voice::Play(WAVE_DATA *pWave, bool loop)
{
	Stop();

	auto wfx = pWave->getWfx();
	m_pXAudio2->CreateSourceVoice(&m_pVoice, &wfx);

	XAUDIO2_BUFFER		buffer;
	ZeroMemory(&buffer, sizeof(buffer));

	buffer.AudioBytes = pWave->getDataSize();
	buffer.pAudioData = pWave->getPCM();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	buffer.LoopCount = (loop) ? XAUDIO2_LOOP_INFINITE : 0;

	m_pVoice->SubmitSourceBuffer(&buffer);
	m_pVoice->Start(0);
}

// 停止
void XA_Voice::Stop()
{
	if (m_pVoice) {
		m_pVoice->Stop(0);
		m_pVoice->FlushSourceBuffers();
		m_pVoice->DestroyVoice();
		m_pVoice = nullptr;
	}
}

//==============================================================================

// 一時停止
void XA_Voice::Pause()
{
	if (m_pVoice) m_pVoice->Stop(0);
}

// 再開
void XA_Voice::Resume()
{
	if (m_pVoice) m_pVoice->Start(0);
}

//==============================================================================

// ボリュームの取得
float XA_Voice::GetVolume()
{
	float volume = 0.0f;
	if (m_pVoice) m_pVoice->GetVolume(&volume);
	return volume;
}

// ボリュームの設定
void XA_Voice::SetVolume(float volume)
{
	if (m_pVoice) {
		if (volume < 0.0f) volume = 0.0f;
		if (volume > 1.0f) volume = 1.0f;
		m_pVoice->SetVolume(volume);
	}
}

//==============================================================================

// ピッチの取得
float XA_Voice::GetFrequency()
{
	float ratio = 1.0f;
	if (m_pVoice) m_pVoice->GetFrequencyRatio(&ratio);
	return ratio;
}

// ピッチの設定
void XA_Voice::SetFrequency(float ratio)
{
	if (m_pVoice) {
		if (ratio < XAUDIO2_MIN_FREQ_RATIO) ratio = XAUDIO2_MIN_FREQ_RATIO;
		if (ratio > XAUDIO2_DEFAULT_FREQ_RATIO) ratio = XAUDIO2_DEFAULT_FREQ_RATIO;
		m_pVoice->SetFrequencyRatio(ratio);
	}
}

//==============================================================================

// 再生中チェック
bool XA_Voice::isPlaying()
{
	if (!m_pVoice) return false;

	XAUDIO2_VOICE_STATE	state;
	m_pVoice->GetState(&state);
	return (state.BuffersQueued != 0);
}

//******************************************************************************
