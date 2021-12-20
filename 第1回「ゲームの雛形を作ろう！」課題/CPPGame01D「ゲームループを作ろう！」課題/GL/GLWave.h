#ifndef _GLWAVE_H_
#define	_GLWAVE_H_
//******************************************************************************
//
//
//		Waveファイル
//
//
//******************************************************************************

#include <windows.h>
#include <mmsystem.h>
#include <vector>
using namespace std;

//==============================================================================
//
//		リソース
//
//==============================================================================

//// Waveファイル
//class GLWave
//{
//private:
//	WAVEFORMATEX	m_wfx;				// フォーマット
//	DWORD			m_dataSize;			// データサイズ
//	BYTE*			m_pPCM;				// PCMデータ
//public:
//	GLWave();
//	~GLWave();
//
//	void load(char *filename = NULL);
//private:
//	void loadPCM(HMMIO hmmio);
//public:
//	void release();
//	WAVEFORMATEX	getWfx() { return m_wfx; }
//	DWORD			getDataSize() { return m_dataSize; }
//	BYTE*			getPCM() { return m_pPCM; }
//};

//==============================================================================
//
//		リソース管理
//
//==============================================================================

#include <map>
using namespace std;

// Waveファイル管理クラス
class GLWaveManager
{
private:
	//vector<GLWave>		m_resource;
	map<char*, WAVE_DATA*>		m_resource;
public:
	static GLWaveManager* getInstance()
	{
		static GLWaveManager instance;
		return &instance;
	}

	void Init();
	void Uninit();

	//void Resize(int n);
	void Load(LOAD_WAVE* pLoad);
	void ReleaseAll();

	//GLWave* getResource(int i);
};

#define pGLWaveManager		(GLWaveManager::getInstance())

//******************************************************************************
#endif // !_GLWAVE_H_