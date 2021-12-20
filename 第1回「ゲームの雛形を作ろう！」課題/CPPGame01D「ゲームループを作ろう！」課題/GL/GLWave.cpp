//******************************************************************************
//
//
//		Waveファイル
//
//
//******************************************************************************

#pragma comment ( lib, "winmm.lib" )

#include "GLAll.h"

//#define GLWAVE_MAX			0x100			// Waveファイル最大数（デフォルト）

//==============================================================================
//
//		リソース
//
//==============================================================================

// コンストラクタ
//GLWave::GLWave()
//{
//	ZeroMemory(this, sizeof(*this));
//}

//==============================================================================

//// デストラクタ
//GLWave::~GLWave()
//{
//	release();
//}

//==============================================================================

//// 解放
//void GLWave::release()
//{
//	if (m_pPCM) {
//		delete[] m_pPCM;
//		m_pPCM = nullptr;
//	}
//}

//==============================================================================

//// ロード
//void GLWave::load(char* filename)
//{
//	release();
//	
//	HMMIO hmmio = mmioOpenA(filename, nullptr, MMIO_ALLOCBUF | MMIO_READ);
//	if (hmmio) {
//		loadPCM(hmmio);
//		mmioClose(hmmio, 0);
//	}
//}

//------------------------------------------------------------------------------

//// PCMデータのロード
//void GLWave::loadPCM(HMMIO hmmio)
//{
//	MMCKINFO		mainChunk;
//	MMCKINFO		subChunk;
//
//	// RIFFチャンクのWAVEタイプ検索
//	mainChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
//	if (mmioDescend(hmmio, &mainChunk, NULL, MMIO_FINDRIFF)) return;
//
//	// fmtチャンクの検索
//	subChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
//	if (mmioDescend(hmmio, &subChunk, &mainChunk, MMIO_FINDCHUNK)) return;
//
//	// フォーマットの読み込み
//	if (mmioRead(hmmio, (HPSTR)&m_wfx, sizeof(m_wfx)) != sizeof(m_wfx)) return;
//	if (m_wfx.wFormatTag != WAVE_FORMAT_PCM) return;
//	m_wfx.cbSize = 0;
//
//	// チャンクを戻る
//	if (mmioAscend(hmmio, &subChunk, 0)) return;
//
//	// dataチャンクの検索
//	subChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
//	if (mmioDescend(hmmio, &subChunk, &mainChunk, MMIO_FINDCHUNK)) return;
//	m_dataSize = subChunk.cksize;
//
//	// データロード
//	m_pPCM = new BYTE[m_dataSize];
//	mmioRead(hmmio, (HPSTR)m_pPCM, m_dataSize);
//}

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// 初期設定
void GLWaveManager::Init()
{
	//Resize(GLWAVE_MAX);
}
// 終了処理
void GLWaveManager::Uninit()
{
	//m_resource.clear();
	ReleaseAll();
}

//==============================================================================

//// 数の変更
//void GLWaveManager::Resize(int n)
//{
//	Uninit();
//	m_resource.resize(n);
//}
// ロード
void GLWaveManager::Load(LOAD_WAVE* pLoad)
{
	for (auto& pGLWave : *pLoad) {
		if (m_resource.find(pGLWave->m_filename) == m_resource.end()) {
			pGLWave->load();
			m_resource[pGLWave->m_filename] = pGLWave;
		}
	}
}
// 解放
void GLWaveManager::ReleaseAll()
{
	//for (auto& res : m_resource) res.release();
	for (auto& item : m_resource) {
		item.second->unload();
	}
	m_resource.clear();
}

//==============================================================================

//// リソースを返す
//GLWave* GLWaveManager::getResource(int i)
//{
//	return ((UINT)i < m_resource.size()) ? &m_resource[i] : nullptr;
//}

//******************************************************************************
