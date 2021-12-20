//******************************************************************************
//
//
//		描画関連処理
//
//
//******************************************************************************

#pragma comment( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )
#pragma comment ( lib, "winmm.lib" )

//------< include >-------------------------------------------------------------

#include "GLAll.h"

//------< extern >--------------------------------------------------------------
//------< define >--------------------------------------------------------------

// フレーム数の定義
#define FPS				60				// フレームレート
#define CHECK_TIME		(6000/FPS)		// 1フレームあたりの時間（単位1/6ミリ秒：(1000/60)*(60/FPS)*6）

//------< structure >-----------------------------------------------------------

// デバイス設定データ構造体
struct SETTING_DATA
{
	D3DDEVTYPE				DevType;					// デバイスタイプ
	DWORD					BehaviorFlags;				// デバイス動作制御フラグ
	D3DFORMAT				DisplayFormat;				// ディスプレイ（バックバッファ）フォーマット
	UINT					RefreshRate;				// リフレッシュレート
};

//------< data >----------------------------------------------------------------

// フルスクリーンモードのデバイス設定データ（優先順）
static SETTING_DATA setting_data[] = {
	{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DFMT_X8R8G8B8, FPS },
	{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DFMT_R5G6B5  , FPS },
	{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DFMT_X8R8G8B8, D3DPRESENT_RATE_DEFAULT },
	{ D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING, D3DFMT_R5G6B5  , D3DPRESENT_RATE_DEFAULT },
	{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DFMT_X8R8G8B8, FPS },
	{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DFMT_R5G6B5  , FPS },
	{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DFMT_X8R8G8B8, D3DPRESENT_RATE_DEFAULT },
	{ D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING, D3DFMT_R5G6B5  , D3DPRESENT_RATE_DEFAULT },
	{ D3DDEVTYPE_REF, 0, D3DFMT_UNKNOWN , 0 },	// 終了コード
};

// デフォルトフォント
static LOAD_FONT load_font[] = {
	{ FONT_S, "ＭＳ ゴシック", 16, true, false },
	{ FONT_M, "ＭＳ ゴシック", 32, true, false },
	{ FONT_L, "ＭＳ ゴシック", 64, true, false },
	{ -1, "", 0, 0, 0 }
};

//------< work >----------------------------------------------------------------

static LPDIRECT3D9				m_pD3D;						// Direct3Dインターフェイス
static LPDIRECT3DDEVICE9		m_pd3dDevice;				// Direct3Dデバイスインターフェイス
static D3DPRESENT_PARAMETERS	m_d3dpp;					// デバイスの動作を設定する構造体
static list<IGLDrawCmd*>		m_drawList[ZCODE_MAX];		// 描画リスト

enum LASTSET
{
	LASTSET_INITSTATE,
	LASTSET_CAMERA,
	LASTSET_LIGHT,
	LASTSET_AMBIENT,
	LASTSET_FOG,
	LASTSET_MAX
};

static IGLDrawCmd*				m_lastSetting[LASTSET_MAX];

//------< prototype >-----------------------------------------------------------

static HRESULT CreateFullScreenDevice(HWND hWnd);
static HRESULT CreateWindowDevice(HWND hWnd);

static void addList(int zcode, IGLDrawCmd* cmd);
static void addSetting(LASTSET i, IGLDrawCmd* cmd);

//==============================================================================
//
//		初期設定＆終了処理
//
//==============================================================================

// 初期設定
HRESULT GLDraw::Init(HWND hWnd, int width, int height, bool isFullscreen)
{
	ZeroMemory(&m_lastSetting, sizeof(m_lastSetting));

	// Direct3Dインターフェイスの取得
	if (NULL == (m_pD3D = Direct3DCreate9(D3D_SDK_VERSION))) return E_FAIL;

	// Direct3Dデバイスインターフェイスの取得
	if (FAILED((isFullscreen) ? CreateFullScreenDevice(hWnd) : CreateWindowDevice(hWnd))) return E_FAIL;

	// リソースの設定
	pGLFontManager->Init();
	pGLTextureManager->Init();
	pGLMeshManager->Init();

	pGLFontManager->Load(m_pd3dDevice, load_font);

	// 描画ステートの初期設定
	addList(ZCODE_MAX - 1, new GLDrawSetInitState);
	addSetting(LASTSET_INITSTATE, new GLDrawSetInitState);

	return S_OK;
}

//------------------------------------------------------------------------------

// フルスクリーンモードのデバイス設定
static HRESULT CreateFullScreenDevice(HWND hWnd)
{
	// デバイス動作の設定
	RECT			rcClient;
	GetClientRect(hWnd, &rcClient);
	m_d3dpp.Windowed = FALSE;							// ウインドウモード（フルスクリーン）
	m_d3dpp.BackBufferWidth = rcClient.right;			// バックバッファの横幅
	m_d3dpp.BackBufferHeight = rcClient.bottom;			// バックバッファの高さ
	m_d3dpp.BackBufferCount = 1;						// バックバッファの数
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;			// スワップエフェクト

	m_d3dpp.EnableAutoDepthStencil = TRUE;				// Zバッファ
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		// バッファのフォーマット
	//m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	// Zバッファのフォーマット

	// セッティングデータにある設定を片っ端から試す
	SETTING_DATA	*setting = setting_data;
	while (setting->DisplayFormat != D3DFMT_UNKNOWN) {

		m_d3dpp.BackBufferFormat = setting->DisplayFormat;			// バックバッファのフォーマット
		m_d3dpp.FullScreen_RefreshRateInHz = setting->RefreshRate;	// リフレッシュレート
		m_d3dpp.PresentationInterval = (setting->RefreshRate == FPS) ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

		if (SUCCEEDED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, setting->DevType, hWnd, setting->BehaviorFlags, &m_d3dpp, &m_pd3dDevice))) return S_OK;
		setting++;
	}

	return E_FAIL;
}

//------------------------------------------------------------------------------

// ウインドウモードのデバイス設定
static HRESULT CreateWindowDevice(HWND hWnd)
{
	//現在のモードを取得
	D3DDISPLAYMODE	dmode;
	m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dmode);

	// デバイス動作の設定
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	m_d3dpp.Windowed = TRUE;							// ウインドウモード（ウインドウ）
	m_d3dpp.BackBufferCount = 1;						// バックバッファの数
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;			// スワップエフェクト
	m_d3dpp.BackBufferFormat = dmode.Format;			// バックバッファのフォーマット

	m_d3dpp.EnableAutoDepthStencil = TRUE;				// Zバッファ
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;		// Zバッファのフォーマット
	//m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;	// Zバッファのフォーマット（ステンシルあり）

	// デバイスの生成
	if (SUCCEEDED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dDevice))) return S_OK;
	return m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &m_d3dpp, &m_pd3dDevice);
}

//==============================================================================

// 終了処理
void GLDraw::Uninit()
{
	pGLMeshManager->Uninit();		// メッシュの解放
	pGLTextureManager->Uninit();	// テクスチャの解放
	pGLFontManager->Uninit();		// フォントの解放

	for (auto& item : m_lastSetting) {
		if (item) delete item;
	}

	// Direct3Dの終了処理
	RELEASE(m_pd3dDevice);
	RELEASE(m_pD3D);
}

//==============================================================================
//
//		描画制御
//
//==============================================================================

// 描画開始
void GLDraw::BegineScene()
{
	m_pd3dDevice->BeginScene();
}

//==============================================================================

// 画面クリア
void GLDraw::ClearScene(int argb)
{
	//m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, argb, 1.0f, 0 );
	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, argb, 1.0f, 0);
}

//==============================================================================

// 描画終了＆表示
void GLDraw::EndScene()
{
	// 描画リストによる描画
	for (int i = ZCODE_MAX - 1; i >= 0; i--) {
		for (auto& cmd : m_drawList[i]) {
			cmd->execute(m_pd3dDevice);
			delete cmd;
		}
		m_drawList[i].clear();
	}

	// 描画終了
	m_pd3dDevice->EndScene();

	// シーンの表示
	switch (m_d3dpp.PresentationInterval) {
	// Vsync同期のとき
	case D3DPRESENT_INTERVAL_ONE:
	case D3DPRESENT_INTERVAL_TWO:
	case D3DPRESENT_INTERVAL_THREE:
	case D3DPRESENT_INTERVAL_FOUR:
		break;
	// タイマーでフレーム更新のとき
	default:
		static DWORD updateTime = timeGetTime() * 6;
		while (timeGetTime() * 6 - updateTime < CHECK_TIME) Sleep(1);		// フレーム時間（16.66*6 = 100）を越えるまで待つ
		updateTime += CHECK_TIME;
		break;
	}

	// バックバッファの表示
	if (m_pd3dDevice->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) {
		// デバイスロストのとき
		if (m_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
			pGLFontManager->OnLostDevice();
			if (SUCCEEDED(m_pd3dDevice->Reset(&m_d3dpp))) {

				// 描画ステートの復元
				for (auto& item : m_lastSetting) {
					if (item) item->execute(m_pd3dDevice);
				}

				pGLFontManager->OnResetDevice();
			}
		}
	}
}

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// テクスチャ
void GLDraw::LoadTexture(LOAD_TEXTURE* pLoad) { pGLTextureManager->Load(m_pd3dDevice, pLoad); }
void GLDraw::ReleaseTexture() { pGLTextureManager->ReleaseAll(); }

// フォント
void GLDraw::ResizeFont(int n) { pGLFontManager->Resize(n); }
void GLDraw::LoadFont(LOAD_FONT* pLoad) { pGLFontManager->Load(m_pd3dDevice, pLoad); }
void GLDraw::ReleaseFont() { pGLFontManager->ReleaseAll(); }

// メッシュ
void GLDraw::ResizeMesh(int n) { pGLMeshManager->Resize(n); }
void GLDraw::LoadMesh(LOAD_MESH* pLoad) { pGLMeshManager->Load(m_pd3dDevice, pLoad); }
void GLDraw::ReleaseMesh() { pGLMeshManager->ReleaseAll(); }

//==============================================================================
//
//		コマンド追加
//
//==============================================================================

// 描画リスト追加
static void addList(int zcode, IGLDrawCmd* cmd)
{
	if ((UINT)zcode < ZCODE_MAX) m_drawList[zcode].push_back(cmd);
}
// セッティング追加
static void addSetting(LASTSET i, IGLDrawCmd* cmd)
{
	if (m_lastSetting[i]) delete m_lastSetting[i];
	m_lastSetting[i] = cmd;
}

//==============================================================================
//
//		描画コマンドの追加
//
//==============================================================================

//-------------------------------
// スプライト描画
//-------------------------------
void GLDraw::DrawSprite(float x, float y, SPRITE_DATA* data, float sx, float sy, float rot, bool xflip, bool yflip, float sh, float sv, int db, int tb, int argb, int zcode)
{
	if (!data) return;

	addList(zcode, new GLDrawSetDB(db));
	addList(zcode, new GLDrawSetTB(tb));
	addList(zcode, new GLDrawTexure(x, y, data, sx, sy, rot, xflip, yflip, sh, sv, argb));
}

//-------------------------------
// 文字列描画
//-------------------------------

void GLDraw::DrawString(int x, int y, int fontNum, const char* str, int color, bool isCenter, int db, int alpha, int count, int zcode)
{
	if (!str) return;
	if (count == 0) return;

	LPD3DXFONT	pFont = pGLFontManager->getResource(fontNum);
	if (!pFont) return;

	addList(zcode, new GLDrawSetDB(db));
	addList(zcode, new GLDrawFont(x, y, pFont, str, (alpha << 24) | (color & 0x00ffffff), isCenter, count));
}

//-------------------------------
// 直線描画
//-------------------------------
void GLDraw::DrawLine(float x0, float y0, float x1, float y1, int color, int db, int alpha, int zcode)
{
	addList(zcode, new GLDrawSetDB(db));
	addList(zcode, new GLDrawLine(x0, y0, x1, y1, (alpha << 24) | (color & 0x00ffffff)));
}

//-------------------------------
// 四角形描画
//-------------------------------
void GLDraw::DrawRectangle(float left, float top, float width, float height, int color, int db, int alpha, int zcode)
{
	addList(zcode, new GLDrawSetDB(db));
	addList(zcode, new GLDrawRectangle(left, top, width, height, (alpha << 24) | (color & 0x00ffffff)));
}

//-------------------------------
// メッシュ描画
//-------------------------------
void GLDraw::DrawMesh(MESH_DATA* data, D3DXMATRIX& matTrans, int db, float alpha, float r, float g, float b, int zcode)
{
	if (!data) return;
	addList(zcode, new GLDrawSetDB(db));
	addList(zcode, new GLDrawMesh(data, matTrans, alpha, r, g, b));
}

//==============================================================================
//
//		描画設定の追加
//
//==============================================================================

//-------------------------------
// カメラ設定
//-------------------------------
void GLDraw::SetCamera(CAMERA_DATA* data, int zcode)
{
	if (!data) return;
	addList(zcode, new GLDrawSetCamera(data));
	addSetting(LASTSET_CAMERA, new GLDrawSetCamera(data));
}

//-------------------------------
// ライト設定
//-------------------------------
void GLDraw::SetLight(LIGHT_DATA* data, int zcode)
{
	addList(zcode, new GLDrawSetLight(data));
	addSetting(LASTSET_LIGHT, new GLDrawSetLight(data));
}

//-------------------------------
// アンビエント設定
//-------------------------------
void GLDraw::SetAmbient(int rgb, int zcode)
{
	addList(zcode, new GLDrawSetAmbient(rgb));
	addSetting(LASTSET_AMBIENT, new GLDrawSetAmbient(rgb));
}

//-------------------------------
// フォグ設定
//-------------------------------
void GLDraw::SetFog(FOG_DATA* data, int zcode)
{
	addList(zcode, new GLDrawSetFog(data));
	addSetting(LASTSET_FOG, new GLDrawSetFog(data));
}

//==============================================================================
//
//		描画コマンド
//
//==============================================================================

// テクスチャブレンダ
GLDrawSetTB::GLDrawSetTB(int tb) : m_tb(tb)
{
}

void GLDrawSetTB::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	// カラーの設定
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);	// ARG1＝テクスチャカラー
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);	// ARG2＝頂点カラー

	switch (m_tb) {
	default:
	case TB_TEX:
		// カラー＝テクスチャカラー
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		break;
	case TB_COLOR:
		//case D3D_TBNOTEX:
		// カラー＝頂点カラー
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		break;
	case TB_ADD:
		// カラー＝テクスチャカラー＋頂点カラー
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		break;
	case TB_SUB:
		// カラー＝テクスチャカラー－頂点カラー
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
		break;
	case TB_MUL:
		// カラー＝テクスチャカラー×頂点カラー
		pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		break;
	}

	// アルファの設定
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
}

//==============================================================================

// ディスティネーションブレンダ
GLDrawSetDB::GLDrawSetDB(int db) : m_db(db)
{
}

void GLDrawSetDB::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	switch (m_db) {
	default:
	case DB_COPY:
		// アルファ＝テクスチャアルファ（頂点アルファは無効）
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		// アルファブレンド無効なので設定しない
		//pd3dDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE           );
		//pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO          );
		//pd3dDevice->SetRenderState(D3DRS_BLENDOP  , D3DBLENDOP_ADD         );
		break;
	case DB_MIX:
		// カラー＝SRCカラー×SRCアルファ＋DSTカラー×（１－SRCアルファ）
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		break;
	case DB_ADD:
		// カラー＝SRCカラー×SRCアルファ＋DSTカラー
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		break;
	case DB_SUB:
		// カラー＝-SRCカラー×SRCアルファ＋DSTカラー
		pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
		break;
	}

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, (m_db == DB_COPY) ? TRUE : FALSE);
}

//==============================================================================

// 直線描画
GLDrawLine::GLDrawLine(float x0, float y0, float x1, float y1, int argb)
{
	v[0].x = x0;
	v[0].y = y0;

	v[1].x = x1;
	v[1].y = y1;

	v[0].z       = v[1].z       = 0.0f;
	v[0].rhw     = v[1].rhw     = 1.0f;
	v[0].diffuse = v[1].diffuse = argb;
}

void GLDrawLine::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetFVF(D3DFVF_2DCOLOR);
	pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, v, sizeof(D3DVERTEX_2DCOLOR));
}

//==============================================================================

// 四角形描画
GLDrawRectangle::GLDrawRectangle(float left, float top, float width, float height, int argb)
{
	v[0].x = left;
	v[0].y = top;

	v[1].x = left + width;
	v[1].y = top;

	v[2].x = left;
	v[2].y = top + height;

	v[3].x = left + width;
	v[3].y = top + height;

	v[0].z       = v[1].z       = v[2].z       = v[3].z       = 0.0f;
	v[0].rhw     = v[1].rhw     = v[2].rhw     = v[3].rhw     = 1.0f;
	v[0].diffuse = v[1].diffuse = v[2].diffuse = v[3].diffuse = argb;
}

void GLDrawRectangle::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetTexture(0, NULL);
	pd3dDevice->SetFVF(D3DFVF_2DCOLOR);
	pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(D3DVERTEX_2DCOLOR));
}

//==============================================================================

// 初期ステート設定
void GLDrawSetInitState::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);			// アルファテスト：有効
	pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0x00);					// 基準アルファ値：0x00
	pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);		// アルファ比較：基準アルファ値より大きいときOK
}

//==============================================================================

// カメラ設定
GLDrawSetCamera::GLDrawSetCamera(CAMERA_DATA* data)
{
	D3DXMatrixLookAtLH(&matView, &data->pos, &data->look, &data->up);
	D3DXMatrixPerspectiveFovLH(&matProj, data->fovy, data->aspect, data->znear, data->zfar);
}
void GLDrawSetCamera::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

//==============================================================================

// ライト設定
GLDrawSetLight::GLDrawSetLight(LIGHT_DATA* data) : m_data(data)
{
}
void GLDrawSetLight::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_data) {
		D3DLIGHT9		light;
		ZeroMemory(&light, sizeof(D3DLIGHT9));
		light.Type = D3DLIGHT_DIRECTIONAL;
		light.Direction = m_data->direction;
		light.Diffuse = light.Specular = m_data->color;
		pd3dDevice->SetLight(0, &light);
		pd3dDevice->LightEnable(0, TRUE);
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	}
	else {
		pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}
}

//==============================================================================

// アンビエント設定
GLDrawSetAmbient::GLDrawSetAmbient(int rgb) : m_rgb(rgb)
{
}
void GLDrawSetAmbient::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, m_rgb);
}

//==============================================================================

// フォグ設定
GLDrawSetFog::GLDrawSetFog(FOG_DATA* data) : m_data(data)
{
}
void GLDrawSetFog::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_data) {
		// 線形補間フォグ
		pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
		// フォグの情報設定
		pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_data->color);
		pd3dDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&m_data->zStart)));
		pd3dDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*)(&m_data->zEnd)));
		// フォグ許可
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	}
	else {
		pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	}
}

//==============================================================================
//
//		その他
//
//==============================================================================

// ビューマトリクスの取得
void GLDraw::GetViewMatrix(D3DXMATRIX *pMatrix)
{
	m_pd3dDevice->GetTransform(D3DTS_VIEW, pMatrix);
}

//==============================================================================

// スクリーン座標への変換
void GLDraw::WorldToScreen(float wx, float wy, float wz, int *sx, int *sy)
{
	// ビューポートの取得
	D3DVIEWPORT9	viewport;
	m_pd3dDevice->GetViewport(&viewport);

	// 変換行列の取得
	D3DXMATRIX	matProj, matView, matWorld;
	m_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	m_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixIdentity(&matWorld);

	// スクリーン座標の取得
	D3DXVECTOR3		screenPos;
	D3DXVECTOR3		worldPos(wx, wy, wz);
	D3DXVec3Project(&screenPos, &worldPos, &viewport, &matProj, &matView, &matWorld);
	*sx = (int)screenPos.x;
	*sy = (int)screenPos.y;
}

//******************************************************************************