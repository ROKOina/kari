#ifndef _GLTYPE_H_
#define _GLTYPE_H_
//******************************************************************************
//
//
//		各種ラベル、データ型定義
//
//
//******************************************************************************

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
using namespace std;

//******************************************************************************
//
//
//		入力関連
//
//
//******************************************************************************

// 入力デバイス番号
enum GLDEVICE
{
	GLKEYBOARD,			// キーボード（マウスも可）
	GLJOYSTICK,			// ジョイスティック
};

// PadStateビット割り当て
#define PAD_UP			0x00000001
#define PAD_DOWN		0x00000002
#define PAD_LEFT		0x00000004
#define PAD_RIGHT		0x00000008
#define PAD_START		0x00000010
#define PAD_SELECT		0x00000020
#define PAD_TRG1		0x00000040
#define PAD_TRG2		0x00000080
#define PAD_TRG3		0x00000100
#define PAD_TRG4		0x00000200
#define PAD_L1			0x00000400
#define PAD_R1			0x00000800
#define PAD_L2			0x00001000
#define PAD_R2			0x00002000
#define PAD_L3			0x00004000
#define PAD_R3			0x00008000

// ジョイスティック用ボタンラベル
enum {
	JOY_BUTTON_01,
	JOY_BUTTON_02,
	JOY_BUTTON_03,
	JOY_BUTTON_04,
	JOY_BUTTON_05,
	JOY_BUTTON_06,
	JOY_BUTTON_07,
	JOY_BUTTON_08,
	JOY_BUTTON_09,
	JOY_BUTTON_10,
	JOY_BUTTON_11,
	JOY_BUTTON_12,
	JOY_BUTTON_13,
	JOY_BUTTON_14,
	JOY_BUTTON_15,
	JOY_BUTTON_16,
};

// ビット割り当てデータ
struct PAD_ASSIGN
{
	int			bit;				// pad_state用ビットデータ
	int			code;				// 対応するキー（ボタン）コード
};

// 入力デバイス割り当て
#pragma warning( disable : 4200 )
struct DEVICE_ASSIGN
{
	GLDEVICE	device;				// 使用するデバイス
	PAD_ASSIGN	assign[];			// ビット割り当てデータ
};
#pragma warning( default : 4200 )

// 入力データ
struct PAD_WORK
{
	int			state;				// 通常入力
	int			trg;				// トリガー入力
	float		volume[4];			// アナログ入力
};

//******************************************************************************
//
//
//		描画関連
//
//
//******************************************************************************
//==============================================================================
//
//		描画設定
//
//==============================================================================

// カラーラベル（0x00RRGGBB）
#define COLOR_BLACK		0x00000000			// 黒
#define COLOR_GRAY		0x00808080			// 灰色
#define COLOR_WHITE		0x00ffffff			// 白

#define COLOR_RED		0x00ff0000			// 赤
#define COLOR_GREEN		0x0000ff00			// 緑
#define COLOR_BLUE		0x000000ff			// 青

#define COLOR_CYAN		0x0000ffff			// 水色
#define COLOR_MAGENTA	0x00ff00ff			// 紫
#define COLOR_YELLOW	0xffffff00			// 黄色

// テクスチャブレンダ設定
enum {
	TB_TEX,			// テクスチャカラーで表示
	TB_COLOR,		// 頂点カラーで表示
	TB_ADD,			// テクスチャカラー＋頂点カラー（ブライト加算）
	TB_SUB,			// テクスチャカラー－頂点カラー（ブライト減算）
	TB_MUL,			// テクスチャカラー×頂点カラー（コントラスト）
};

// ディスティネーションブレンダ設定
enum {
	DB_COPY,		// 通常（透明色あり）
	DB_MIX,			// 半透明（アルファブレンド）
	DB_ADD,			// カラー加算
	DB_SUB,			// カラー減算
};

//==============================================================================
//
//		テクスチャ関連
//
//==============================================================================

// テクスチャデータ
struct TEXTURE_DATA
{
	LPDIRECT3DTEXTURE9	m_pd3dTexture;
	const char*			m_filename;			// ファイル名
	int					m_colorKey;			// カラーキー
	float				m_width;			// 横幅
	float				m_height;			// 高さ

	TEXTURE_DATA(const char* filename, int colorKey);
	~TEXTURE_DATA();
	void load(LPDIRECT3DDEVICE9 pd3dDevice);
	void unload();
};

// 透明色設定なし（テクスチャのアルファをそのまま使用）
struct TEXTURE_NORMAL : public TEXTURE_DATA
{
	TEXTURE_NORMAL(const char* filename);
};

// 「黒」を透明色にする
struct TEXTURE_BLACK : public TEXTURE_DATA
{
	TEXTURE_BLACK(const char* filename);
};

// 「テクスチャの左上の色と同じ色」を透明色にする
struct TEXTURE_LEFTTOP : public TEXTURE_DATA
{
	TEXTURE_LEFTTOP(const char* filename);
};

//==============================================================================

// ロードデータ
typedef vector<TEXTURE_DATA*>	LOAD_TEXTURE;

//==============================================================================

// 画像反転フラグ
#define SPR_XFLIP		0x01
#define	SPR_YFLIP		0x02

// スプライトデータ
struct SPRITE_DATA
{
	TEXTURE_DATA*	m_pTexture;
	float			m_left, m_top;
	float			m_width, m_height;
	float			m_xofs, m_yofs;
	int				m_flip;

	SPRITE_DATA(TEXTURE_DATA* pTexture, float left, float top, float width, float height, float xofs = 0.0f, float yofs = 0.0f, int flip = 0x00);
};

// スプライトデータ（基準点：左上）
struct SPRITE_LEFTTOP : public SPRITE_DATA
{
	SPRITE_LEFTTOP(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip = 0x00);
};

// スプライトデータ（基準点：中心）
struct SPRITE_CENTER : public SPRITE_DATA
{
	SPRITE_CENTER(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip = 0x00);
};

// スプライトデータ（基準点：足元）
struct SPRITE_BOTTOM : public SPRITE_DATA
{
	SPRITE_BOTTOM(TEXTURE_DATA* pTexture, float left, float top, float width, float height, int flip = 0x00);
};

//==============================================================================
//
//		文字列
//
//==============================================================================

// フォント生成データ
struct LOAD_FONT
{
	int			fontNum;		// フォント番号
	const char*	fontname;		// フォント名
	int			size;			// サイズ（高さ）
	bool		bold;			// 太字
	bool		italic;			// 斜体
};

#define FONT_S		0
#define FONT_M		1
#define FONT_L		2

// 文字列データ
struct STRING_DATA
{
	int		fontNum;
	char*	str;
	int		color;
	bool	isCenter;

	STRING_DATA(int fontNum, char* str, int color = COLOR_WHITE, bool isCenter = false);
};

// 文字列データ（左詰め）
struct STRING_LEFT : public STRING_DATA
{
	STRING_LEFT(int fontNum, char* str, int color = COLOR_WHITE);
};
struct STRING_LEFT_S : public STRING_LEFT
{
	STRING_LEFT_S(char* str, int color = COLOR_WHITE);
};
struct STRING_LEFT_M : public STRING_LEFT
{
	STRING_LEFT_M(char* str, int color = COLOR_WHITE);
};
struct STRING_LEFT_L : public STRING_LEFT
{
	STRING_LEFT_L(char* str, int color = COLOR_WHITE);
};

// 文字列データ（センタリング）
struct STRING_CENTER : public STRING_DATA
{
	STRING_CENTER(int fontNum, char*str, int color = COLOR_WHITE);
};
struct STRING_CENTER_S : public STRING_CENTER
{
	STRING_CENTER_S(char*str, int color = COLOR_WHITE);
};
struct STRING_CENTER_M : public STRING_CENTER
{
	STRING_CENTER_M(char*str, int color = COLOR_WHITE);
};
struct STRING_CENTER_L : public STRING_CENTER
{
	STRING_CENTER_L(char*str, int color = COLOR_WHITE);
};

//==============================================================================
//
//		メッシュ
//
//==============================================================================

// メッシュロードデータ
struct LOAD_MESH
{
	int		meshNum;		// メッシュ番号
	char*	filename;		// ファイル名
};

// メッシュデータ
struct MESH_DATA
{
	int				meshNum;			// メッシュ番号
	D3DXMATRIX		matData;			// データ補正行列

	MESH_DATA(int meshNum, float scale = 1.0f, float rot = 0.0f);
	MESH_DATA(int meshNum, D3DXVECTOR3& scale, D3DXVECTOR3& rot, D3DXVECTOR3& trans);
private:
	// データ補正行列の設定
	void SetMatrix(const D3DXVECTOR3& scale = D3DXVECTOR3(1, 1, 1), const D3DXVECTOR3& rot = D3DXVECTOR3(0, 0, 0), const D3DXVECTOR3& trans = D3DXVECTOR3(0, 0, 0));
};

//==============================================================================
//
//		描画設定
//
//==============================================================================

// カメラデータ
struct CAMERA_DATA
{
	// ビューマトリクス
	D3DXVECTOR3		pos;				// カメラの座標
	D3DXVECTOR3		look;				// 注視点
	D3DXVECTOR3		up;					// カメラの回転（上方向ベクトル）
	// プロジェクション
	float			fovy;				// Y方向視野角
	float			aspect;				// アスペクト比（横幅÷高さ）
	float			znear, zfar;		// ニアプレーン、ファープレーン
};

// ライトデータ
struct LIGHT_DATA
{
	D3DVECTOR		direction;			// 向き
	D3DCOLORVALUE	color;				// 色（ディフューズ、スペキュラー）
};

// フォグデータ
struct FOG_DATA
{
	D3DCOLOR		color;				// フォグの色
	float			zStart;				// フォグ開始位置
	float			zEnd;				// フォグ終了位置
};

//******************************************************************************
//
//
//		サウンド関連
//
//
//******************************************************************************

//struct LOAD_WAVE
//{
//	int		waveNum;		// Wave番号
//	char*	filename;		// ファイル名
//};

//// サウンドデータ
//struct WAVE_DATA
//{
//	char*	m_filename;			// ファイル名
//	WAVE_DATA(char* filename);
//	~WAVE_DATA();
//	void load();
//	void unload();
//};

// Waveファイル
struct WAVE_DATA
{
	char*			m_filename;			// ファイル名
	WAVEFORMATEX	m_wfx;				// フォーマット
	DWORD			m_dataSize;			// データサイズ
	BYTE*			m_pPCM;				// PCMデータ

	WAVE_DATA(char* filename);
	~WAVE_DATA();
	void load();
	void unload();
	void loadPCM(HMMIO hmmio);

	WAVEFORMATEX	getWfx() { return m_wfx; }
	DWORD			getDataSize() { return m_dataSize; }
	BYTE*			getPCM() { return m_pPCM; }
};

// ロードデータ
typedef vector<WAVE_DATA*>	LOAD_WAVE;

//******************************************************************************
#endif		// #ifndef _GLTYPE_H_
