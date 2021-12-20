//******************************************************************************
//
//
//		各種ラベル、データ型定義
//
//
//******************************************************************************

#include "GLType.h"

//******************************************************************************
//
//
//		描画関連
//
//
//******************************************************************************
//==============================================================================
//
//		テクスチャ
//
//==============================================================================

// カラーキーラベル
enum {
	COLORKEY_NONE,			// 透明色なし
	COLORKEY_BLACK,			// 「黒」が透明
	COLORKEY_LEFTTOP,		// 「画像の左上と同じ色」が透明
};

// コンストラクタ
TEXTURE_DATA::TEXTURE_DATA(const char* filename, int colorKey) : m_pd3dTexture(0), m_filename(filename), m_colorKey(colorKey)
{
}

TEXTURE_NORMAL::TEXTURE_NORMAL(const char* filename) : TEXTURE_DATA(filename, COLORKEY_NONE) {}
TEXTURE_BLACK::TEXTURE_BLACK(const char* filename) : TEXTURE_DATA(filename, COLORKEY_BLACK) {}
TEXTURE_LEFTTOP::TEXTURE_LEFTTOP(const char* filename) : TEXTURE_DATA(filename, COLORKEY_LEFTTOP) {}

//==============================================================================

// デストラクタ
TEXTURE_DATA::~TEXTURE_DATA()
{
	unload();
}

//==============================================================================

// テクスチャのロード
void TEXTURE_DATA::load(LPDIRECT3DDEVICE9 pd3dDevice)
{
	unload();

	// 画像のロードとテンポラリテクスチャの作成
	LPDIRECT3DTEXTURE9	pTmpTexture;		// テンポラリテクスチャ
	if (FAILED(D3DXCreateTextureFromFileExA(pd3dDevice, m_filename, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &pTmpTexture))) return;

	// カラーキーの設定
	IDirect3DSurface9	*pTmpSurface;		// テンポラリテクスチャのサーフェイス
	D3DLOCKED_RECT		LockedRect;			// サーフェイスロック領域
	D3DCOLOR			ColorKey;			// カラーキー

	pTmpTexture->GetSurfaceLevel(0, &pTmpSurface);		// テンポラリテクスチャのサーフェイス取得

	switch (m_colorKey) {
	case COLORKEY_BLACK:		// 黒を透明にする場合
		ColorKey = D3DCOLOR_ARGB(255, 0, 0, 0);			// 「不透明な黒」
		break;
	case COLORKEY_LEFTTOP:		// 画像左上と同じ色を透明にする場合
		pTmpSurface->LockRect(&LockedRect, NULL, D3DLOCK_READONLY);
		ColorKey = *((D3DCOLOR *)LockedRect.pBits);			// 「画像の左上の色」取得
		pTmpSurface->UnlockRect();
		break;
	default:					// それ以外
		ColorKey = D3DCOLOR_ARGB(0, 0, 0, 0);;			// 「透明な黒」
		break;
	}

	// 本来のテクスチャ作成
	D3DSURFACE_DESC		desc;				// テンポラリテクスチャのサーフェイス情報
	pTmpSurface->GetDesc(&desc);			// サーフェイス情報取得
	m_width = (float)desc.Width;
	m_height = (float)desc.Height;

	D3DXCreateTexture(pd3dDevice, desc.Width, desc.Height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pd3dTexture);

	// 画像転送（テンポラリ→本来）
	IDirect3DSurface9	*pSurface;
	m_pd3dTexture->GetSurfaceLevel(0, &pSurface);		// サーフェイス取得
	D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pTmpSurface, NULL, NULL, D3DX_FILTER_NONE, ColorKey);	// 転送
	pSurface->Release();

	// テンポラリの削除
	pTmpSurface->Release();							// テンポラリテクスチャのサーフェイス解放
	pTmpTexture->Release();							// テンポラリテクスチャの解放
}

//==============================================================================

// テクスチャの解放
void TEXTURE_DATA::unload()
{
	if (m_pd3dTexture) {
		m_pd3dTexture->Release();
		m_pd3dTexture = nullptr;
	}
}

//==============================================================================
//
//		スプライトデータ
//
//==============================================================================

// コンストラクタ
SPRITE_DATA::SPRITE_DATA(TEXTURE_DATA* pTexture, float left, float top, float width, float height, float xofs, float yofs, int flip)
	: m_pTexture(pTexture), m_left(left), m_top(top), m_width(width), m_height(height), m_xofs(xofs), m_yofs(yofs), m_flip(flip)
{
}

SPRITE_LEFTTOP::SPRITE_LEFTTOP(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip) : SPRITE_DATA(pTexture, left, top, width, height, 0, 0, flip) {}
SPRITE_CENTER::SPRITE_CENTER(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip) : SPRITE_DATA(pTexture, left, top, width, height, -width / 2, -height / 2, flip) {}
SPRITE_BOTTOM::SPRITE_BOTTOM(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip) : SPRITE_DATA(pTexture, left, top, width, height, -width / 2, -height, flip) {}

//==============================================================================
//
//		文字列データ
//
//==============================================================================

// 文字列データ
STRING_DATA::STRING_DATA(int fontNum, char* str, int color, bool isCenter)
{
	this->fontNum = fontNum;
	this->str = str;
	this->color = color;
	this->isCenter = isCenter;
}

// 文字列データ（左詰め）
STRING_LEFT::STRING_LEFT(int fontNum, char* str, int color) : STRING_DATA(fontNum, str, color, false)
{
}
STRING_LEFT_S::STRING_LEFT_S(char* str, int color) : STRING_LEFT(FONT_S, str, color) {}
STRING_LEFT_M::STRING_LEFT_M(char* str, int color) : STRING_LEFT(FONT_M, str, color) {}
STRING_LEFT_L::STRING_LEFT_L(char* str, int color) : STRING_LEFT(FONT_L, str, color) {}

// 文字列データ（センタリング）
STRING_CENTER::STRING_CENTER(int fontNum, char* str, int color) : STRING_DATA(fontNum, str, color, true)
{
}
STRING_CENTER_S::STRING_CENTER_S(char* str, int color) : STRING_CENTER(FONT_S, str, color) {}
STRING_CENTER_M::STRING_CENTER_M(char* str, int color) : STRING_CENTER(FONT_M, str, color) {}
STRING_CENTER_L::STRING_CENTER_L(char* str, int color) : STRING_CENTER(FONT_L, str, color) {}

//==============================================================================
//
//		メッシュデータ
//
//==============================================================================

// コンストラクタ（Y軸回転とスケールの指定）
MESH_DATA::MESH_DATA(int meshNum, float scale, float rot)
{
	this->meshNum = meshNum;
	SetMatrix(D3DXVECTOR3(scale, scale, scale), D3DXVECTOR3(0.0f, rot, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}

// コンストラクタ（フル指定）
MESH_DATA::MESH_DATA(int meshNum, D3DXVECTOR3& scale, D3DXVECTOR3& rot, D3DXVECTOR3& trans)
{
	this->meshNum = meshNum;
	SetMatrix(scale, rot, trans);
}

// データ補正行列の設定
void MESH_DATA::SetMatrix(const D3DXVECTOR3& scale, const D3DXVECTOR3& rot, const D3DXVECTOR3& trans)
{
	D3DXMATRIX	matTmp;
	D3DXMatrixIdentity(&matData);
	matData *= *D3DXMatrixScaling(&matTmp, scale.x, scale.y, scale.z);
	matData *= *D3DXMatrixRotationYawPitchRoll(&matTmp, rot.y, rot.x, rot.z);
	matData._41 = trans.x;
	matData._42 = trans.y;
	matData._43 = trans.z;
}

//==============================================================================
//
//		Waveデータ
//
//==============================================================================

WAVE_DATA::WAVE_DATA(char * filename) : m_filename(filename), m_pPCM(0)
{
}

WAVE_DATA::~WAVE_DATA()
{
	unload();
}

void WAVE_DATA::load()
{
	unload();

	HMMIO hmmio = mmioOpenA(m_filename, nullptr, MMIO_ALLOCBUF | MMIO_READ);
	if (hmmio) {
		loadPCM(hmmio);
		mmioClose(hmmio, 0);
	}
}

void WAVE_DATA::unload()
{
	if (m_pPCM) {
		delete[] m_pPCM;
		m_pPCM = nullptr;
	}
}

// PCMデータのロード
void WAVE_DATA::loadPCM(HMMIO hmmio)
{
	MMCKINFO		mainChunk;
	MMCKINFO		subChunk;

	// RIFFチャンクのWAVEタイプ検索
	mainChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(hmmio, &mainChunk, NULL, MMIO_FINDRIFF)) return;

	// fmtチャンクの検索
	subChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hmmio, &subChunk, &mainChunk, MMIO_FINDCHUNK)) return;

	// フォーマットの読み込み
	if (mmioRead(hmmio, (HPSTR)&m_wfx, sizeof(m_wfx)) != sizeof(m_wfx)) return;
	if (m_wfx.wFormatTag != WAVE_FORMAT_PCM) return;
	m_wfx.cbSize = 0;

	// チャンクを戻る
	if (mmioAscend(hmmio, &subChunk, 0)) return;

	// dataチャンクの検索
	subChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hmmio, &subChunk, &mainChunk, MMIO_FINDCHUNK)) return;
	m_dataSize = subChunk.cksize;

	// データロード
	m_pPCM = new BYTE[m_dataSize];
	mmioRead(hmmio, (HPSTR)m_pPCM, m_dataSize);
}

//******************************************************************************
