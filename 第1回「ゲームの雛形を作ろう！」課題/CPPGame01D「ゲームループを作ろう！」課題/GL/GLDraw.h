#ifndef _GLDRAW_H_
#define	_GLDRAW_H_
//******************************************************************************
//
//
//		描画関連処理
//
//
//******************************************************************************

#include <list>
using namespace std;

//==============================================================================
//
//		共通ラベル、構造体、インターフェイス定義
//
//==============================================================================

//--------------------------------
// 頂点データ
//--------------------------------

// 座標変換しない、ライティングしない
struct D3DVERTEX_2DCOLOR
{
	float	x, y, z, rhw;		// 座標変換しない（スクリーン座標で指定）
	DWORD	diffuse;			// 頂点色
};
#define D3DFVF_2DCOLOR		( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

// 2Dテクスチャの頂点データ
struct D3DVERTEX_2DTEXTURE {
	float	x, y, z, rhw;		// 座標変換しない（スクリーン座標で指定）
	DWORD	argb;				// 頂点色
	float	tu1, tv1;			// テクスチャUV座標
};
#define D3DFVF_2DTEXTURE 	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

// 座標変換あり、ライティングしない
struct D3DVERTEX_3DCOLOR
{
	float	x, y, z;			// 座標変換する
	DWORD	diffuse;			// 頂点色
};
#define D3DFVF_3DCOLOR		( D3DFVF_XYZ | D3DFVF_DIFFUSE )

// 座標変換あり、ライティングあり
struct D3DVERTEX_3DNORMAL
{
	D3DXVECTOR3		position;		// 座標
	D3DXVECTOR3		normal;			// 法線ベクトル
};
#define D3DFVF_3DNORMAL		( D3DFVF_XYZ | D3DFVF_NORMAL )


//--------------------------------
// 描画関連インターフェース
//--------------------------------

// 描画コマンド
class IGLDrawCmd
{
public:
	virtual ~IGLDrawCmd() {}
	virtual void execute(LPDIRECT3DDEVICE9 pd3dDevice) = 0;
};

//==============================================================================
//
//		描画管理クラス
//
//==============================================================================

#define ZCODE_MAX			256					// Zコード（奥行き）最大数

namespace GLDraw
{
	//--------------------------------
	// 初期設定＆終了処理
	//--------------------------------
	HRESULT Init(HWND hWnd, int width, int height, bool isFullscreen);
	void Uninit();

	//--------------------------------
	// 描画制御
	//--------------------------------
	void BegineScene();
	void ClearScene(int argb);
	void EndScene();

	//--------------------------------
	// リソース管理
	//--------------------------------
	// テクスチャ管理
	void LoadTexture(LOAD_TEXTURE* pLoad);
	void ReleaseTexture();

	// フォント管理
	void ResizeFont(int n);
	void LoadFont(LOAD_FONT* pLoad);
	void ReleaseFont();
	// メッシュ管理
	void ResizeMesh(int n);
	void LoadMesh(LOAD_MESH* pLoad);
	void ReleaseMesh();

	//--------------------------------
	// 描画コマンドの追加
	//--------------------------------
	void DrawSprite(float x, float y, SPRITE_DATA* data, float sx = 1.0f, float sy = 1.0f, float rot = 0.0f, bool xflip = false, bool yflip = false, float sh = 1.0f, float sv = 1.0f, int db = DB_COPY, int tb = TB_TEX, int argb = 0xffffffff, int zcode = 0);
	void DrawString(int x, int y, int fontNum, const char* str, int color = COLOR_WHITE, bool isCenter = false, int db = DB_COPY, int alpha = 255, int count = -1, int zcode = 0);
	void DrawLine(float x0, float y0, float x1, float y1, int color = COLOR_WHITE, int db = DB_COPY, int alpha = 255, int zcode = 0);
	void DrawRectangle(float left, float top, float width, float height, int color = COLOR_WHITE, int db = DB_COPY, int alpha = 255, int zcode = 0);
	void DrawMesh(MESH_DATA* data, D3DXMATRIX& matTrans, int db = DB_COPY, float alpha = 1.0f, float r = 0.0f, float g = 0.0f, float b = 0.0f, int zcode = 255);

	//--------------------------------
	// 描画設定の追加
	//--------------------------------
	void SetCamera(CAMERA_DATA* data, int zcode = 255);
	void SetLight(LIGHT_DATA* data, int zcode = 255);
	void SetAmbient(int rgb, int zcode = 255);
	void SetFog(FOG_DATA* data, int zcode = 255);

	//--------------------------------
	// その他
	//--------------------------------
	// レンダリングステートの設定
	void GetViewMatrix(D3DXMATRIX* pMatrix);
	void WorldToScreen(float wx, float wy, float wz, int *sx, int *sy);
};

//==============================================================================
//
//		描画コマンド
//
//==============================================================================

//--------------------------------
// テクスチャブレンダ設定
//--------------------------------
class GLDrawSetTB : public IGLDrawCmd
{
private:
	int		m_tb;
public:
	GLDrawSetTB(int tb);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// ディスティネーションブレンダ設定
//--------------------------------
class GLDrawSetDB : public IGLDrawCmd
{
private:
	int		m_db;
public:
	GLDrawSetDB(int db);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// 直線描画
//--------------------------------
class GLDrawLine : public IGLDrawCmd
{
private:
	D3DVERTEX_2DCOLOR v[2];
public:
	GLDrawLine(float x0, float y0, float x1, float y1, int argb);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// 四角形描画
//--------------------------------
class GLDrawRectangle : public IGLDrawCmd
{
private:
	D3DVERTEX_2DCOLOR v[4];
public:
	GLDrawRectangle(float left, float top, float width, float height, int argb = COLOR_WHITE);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// 初期ステート設定
//--------------------------------
class GLDrawSetInitState : public IGLDrawCmd
{
public:
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// カメラ設定
//--------------------------------
class GLDrawSetCamera : public IGLDrawCmd
{
private:
	D3DXMATRIX		matView;
	D3DXMATRIX		matProj;
public:
	GLDrawSetCamera(CAMERA_DATA* data);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// ライト設定
//--------------------------------
class GLDrawSetLight : public IGLDrawCmd
{
private:
	LIGHT_DATA*		m_data;
public:
	GLDrawSetLight(LIGHT_DATA* data);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// アンビエント設定
//--------------------------------
class GLDrawSetAmbient : public IGLDrawCmd
{
private:
	int				m_rgb;
public:
	GLDrawSetAmbient(int rgb);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//--------------------------------
// フォグ設定
//--------------------------------
class GLDrawSetFog : public IGLDrawCmd
{
private:
	FOG_DATA*		m_data;
public:
	GLDrawSetFog(FOG_DATA* data);
	void execute(LPDIRECT3DDEVICE9 pd3dDevice);
};

//******************************************************************************
#endif // !_GLDRAW_H_
