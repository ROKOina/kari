#ifndef _GLTEXTURE_H_
#define	_GLTEXTURE_H_
//******************************************************************************
//
//
//		テクスチャ
//
//
//******************************************************************************

#include <map>
using namespace std;

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// テクスチャ管理クラス
class GLTextureManager
{
private:
	//list<TEXTURE_DATA*>		m_resource;
	map<const char*, TEXTURE_DATA*>		m_resource;
public:
	static GLTextureManager* getInstance()
	{
		static GLTextureManager instance;
		return &instance;
	}

	void Init();
	void Uninit();
	void Load(LPDIRECT3DDEVICE9 pd3dDevice, LOAD_TEXTURE* pLoad);
	void ReleaseAll();
};

#define pGLTextureManager		(GLTextureManager::getInstance())

//==============================================================================
//
//		描画コマンド
//
//==============================================================================

// テクスチャ描画
class GLDrawTexure : public IGLDrawCmd
{
private:
	LPDIRECT3DTEXTURE9	m_pd3dTexture;
	D3DVERTEX_2DTEXTURE	m_v[4];
public:
	GLDrawTexure(float x, float y, SPRITE_DATA* data, float sx, float sy, float rot, bool xflip, bool yflip, float sh, float sv, int argb);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//******************************************************************************
#endif // !_GLTEXTURE_H_