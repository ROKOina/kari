//******************************************************************************
//
//
//		文字列描画
//
//
//******************************************************************************

#include "GLAll.h"

#define GLFONT_MAX			3			// フォント数（デフォルト値）

//==============================================================================
//
//		リソース
//
//==============================================================================

// コンストラクタ
GLFont::GLFont() : pFont(0)
{
}

//==============================================================================

// デストラクタ
GLFont::~GLFont()
{
	release();			// 念のため
}

//==============================================================================

// フォントのロード
void GLFont::load(LPDIRECT3DDEVICE9 pd3dDevice, const char* fontname, int size, bool bold, bool italic)
{
	release();
	D3DXCreateFontA(pd3dDevice, -size, 0, (bold) ? FW_BOLD : FW_NORMAL, 1, italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, fontname, &pFont);
}

//==============================================================================

// フォントの解放
void GLFont::release()
{
	RELEASE(pFont);
}

//==============================================================================

// デバイスロスト時の処理
void GLFont::OnLostDevice()
{
	if (pFont) pFont->OnLostDevice();
}

//==============================================================================

// リセット時の処理
void GLFont::OnResetDevice()
{
	if (pFont) pFont->OnResetDevice();
}

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// 初期設定
void GLFontManager::Init()
{
	Resize(GLFONT_MAX);
}
// 終了処理
void GLFontManager::Uninit()
{
	m_resource.clear();
}

//==============================================================================

// 数の変更
void GLFontManager::Resize(int n) {
	Uninit();				// 念のため
	m_resource.resize(n);
}
// ロード
void GLFontManager::Load(LPDIRECT3DDEVICE9 pd3dDevice, LOAD_FONT* pLoad)
{
	while ((UINT)pLoad->fontNum < m_resource.size()) {
		m_resource[pLoad->fontNum].load(pd3dDevice, pLoad->fontname, pLoad->size, pLoad->bold, pLoad->italic);
		pLoad++;
	}
}
// 解放
void GLFontManager::ReleaseAll()
{
	for (auto& res : m_resource) res.release();
}

//==============================================================================

// デバイスロスト時の処理
void GLFontManager::OnLostDevice()
{
	for (auto& res : m_resource) res.OnLostDevice();
}
// デバイスリセット時の処理
void GLFontManager::OnResetDevice()
{
	for (auto& res : m_resource) res.OnResetDevice();
}

//==============================================================================

// リソースを返す
LPD3DXFONT GLFontManager::getResource(int i)
{
	return ((UINT)i < m_resource.size()) ? m_resource[i].pFont : nullptr;
}

//==============================================================================
//
//		描画コマンド
//
//==============================================================================

// 文字列
GLDrawFont::GLDrawFont(int x, int y, LPD3DXFONT pFont, const char* str, int argb, bool isCenter, int count) : m_pFont(pFont), m_str(str), m_argb(argb), m_count(count)
{
	// 描画座標設定
	m_rc = { x, y, 0, 0 };
	if (isCenter) {
		RECT	size = { 0, 0, 0, 0 };
		m_pFont->DrawTextA(NULL, m_str, -1, &size, DT_CALCRECT, 0);	// 文字列サイズの取得
		m_rc.left -= size.right / 2;
	}
}

void GLDrawFont::execute(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (m_pFont) m_pFont->DrawTextA(NULL, m_str, m_count, &m_rc, DT_LEFT | DT_NOCLIP, m_argb);
}

//******************************************************************************
