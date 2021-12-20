#ifndef _GL_H_
#define	_GL_H_
//******************************************************************************
//
//
//		ライブラリインターフェイス
//
//
//******************************************************************************

#include <windows.h>
#include <tchar.h>

#include "GLType.h"		// 各種ラベル、データ型定義

namespace GL
{
//-------------------------------
// 共通ラベル
//-------------------------------

#define	PI		3.1415926535898f

//==============================================================================
//
//		ライブラリ初期設定＆終了処理、Windows APIなど
//
//==============================================================================

// ライブラリ初期設定
void Init( LPCTSTR lpCaption, int width=640, int height=480, bool isFullscreen=true );

// ライブラリ終了処理
void End();

// ゲームループ続行チェック（実態はメッセージループ）
bool GameLoop();

// カーソル表示
void CursorOn();

// カーソル消去
void CursorOff();

//==============================================================================
//
//		入力処理
//
//==============================================================================

// 入力デバイスの設定
void SetDevice(DEVICE_ASSIGN*** pppDevice);

// 入力情報の更新
void Input();

// 入力ワークのアドレス取得
PAD_WORK* GetPadWork(int i = 0);

#define pad_w			(GetPadWork(0))

#define pad_state		(pad_w->state)
#define pad_trg			(pad_w->trg)
#define pad_volumeX		(pad_w->volume[0])
#define pad_volumeY		(pad_w->volume[1])
#define pad_volumeZ		(pad_w->volume[2])
#define pad_volumeRZ	(pad_w->volume[3])

#define pad_state1		(pad_w[0].state)
#define pad_trg1		(pad_w[0].trg)

#define pad_state2		(pad_w[1].state)
#define pad_trg2		(pad_w[1].trg)

#define pad_state3		(pad_w[2].state)
#define pad_trg3		(pad_w[2].trg)

#define pad_state4		(pad_w[3].state)
#define pad_trg4		(pad_w[3].trg)

// カーソル座標の取得
int GetCursorPosX();
int GetCursorPosY();

#define mouseX			(GetCursorPosX())
#define mouseY			(GetCursorPosY())

//==============================================================================
//
//		描画処理
//
//==============================================================================

//--------------------------------
// シーン制御
//--------------------------------

// 描画開始
void BeginScene();

// シーンクリア
void ClearScene(int argb = COLOR_BLACK);

// 描画終了＆表示
void EndScene();

//--------------------------------
// リソース管理
//--------------------------------

// テクスチャ
void LoadTexture(LOAD_TEXTURE* pLoad);		// ロード
void ReleaseTexture();						// 解放

// フォント
void ResizeFont(int n);						// 数の変更
void LoadFont(LOAD_FONT* pLoad);			// ロード
void ReleaseFont();							// 解放

// メッシュ
void ResizeMesh(int n);						// 数の変更
void LoadMesh(LOAD_MESH* pLoad);			// ロード
void ReleaseMesh();							// 解放

//--------------------------------
// 描画
//--------------------------------

// スプライト描画
void DrawSprite(
	float x, float y,									// スクリーン座標
	SPRITE_DATA* data,									// スプライトデータ
	float sx = 1.0f, float sy = 1.0f,					// 拡大率（画像方向）
	float rot = 0.0f,									// 回転角度（ラジアン）
	bool xflip = false, bool yflip = false,				// 画像反転フラグ
	float sh = 1.0f, float sv = 1.0f,					// 拡大率（画面方向）
	int db = DB_COPY, int alpha = 255,					// ディスティネーションブレンダ設定
	int tb = TB_TEX, int r = 0, int g = 0, int b = 0,	// テクスチャブレンダ設定
	int zcode = 0										// Zコード（描画優先順位）
);

// 文字列描画
void DrawString(
	int x, int y,										// スクリーン座標
	int fontNum,										// フォント番号
	const char* str,											// 文字列
	int color = COLOR_WHITE,							// カラー
	bool isCenter = false,								// センタリングフラグ（false：左詰め、true：センタリング）
	int db = DB_COPY, int alpha = 255,					// ディスティネーションブレンダ設定
	int count = -1,										// 描画文字数（-1：文字列すべて）
	int zcode = 0										// Zコード（描画優先順位）
);
// 文字列描画（Sサイズ、Mサイズ、Lサイズ）
inline void DrawStringS(int x, int y, const char* str, int color = COLOR_WHITE, bool isCenter = false, int db = DB_COPY, int alpha = 255, int count = -1, int zcode = 0)
{
	DrawString(x, y, FONT_S, str, color, isCenter, db, alpha, count, zcode);
}
inline void DrawStringM(int x, int y, const char* str, int color = COLOR_WHITE, bool isCenter = false, int db = DB_COPY, int alpha = 255, int count = -1, int zcode = 0)
{
	DrawString(x, y, FONT_M, str, color, isCenter, db, alpha, count, zcode);
}
inline void DrawStringL(int x, int y, const char* str, int color = COLOR_WHITE, bool isCenter = false, int db = DB_COPY, int alpha = 255, int count = -1, int zcode = 0)
{
	DrawString(x, y, FONT_L, str, color, isCenter, db, alpha, count, zcode);
}

// 文字列描画（STRING_DATA使用）
void DrawString(
	int x, int y,										// スクリーン座標
	STRING_DATA* data,									// 文字列データ
	int db = DB_COPY, int alpha = 255,					// ディスティネーションブレンダ設定
	int count = -1,										// 描画文字数（-1：文字列すべて）
	int zcode = 0										// Zコード（描画優先順位）
);

// 直線描画
void DrawLine(
	float x0, float y0,									// 始点
	float x1, float y1,									// 終点
	int color = COLOR_WHITE,							// カラー
	int db = DB_COPY, int alpha = 255,					// ディスティネーションブレンダ設定
	int zcode = 0										// Zコード（描画優先順位）
);

// 四角形描画
void DrawRectangle(
	float left, float top,								// 左上座標
	float width, float height,							// 横幅、高さ
	int color = COLOR_WHITE,							// カラー
	int db = DB_COPY, int alpha = 255,					// ディスティネーションブレンダ設定
	int zcode = 0										// Zコード（描画優先順位）
);

// メッシュの描画
void DrawMesh(
	MESH_DATA *data,									// メッシュデータ
	D3DXMATRIX& matTrans,								// ワールド変換行列
	int db = DB_COPY, float alpha = 1.0f,				// ディスティネーションブレンダ設定
	float r = 0.0f, float g = 0.0f, float b = 0.0f,		// カラー加算値
	int zcode = 255										// Zコード（描画優先順位）
);

//--------------------------------
// 描画設定
//--------------------------------

// カメラの設定
void SetCamera(CAMERA_DATA* data, int zcode = 255);

// ライトの設定
void SetLight(LIGHT_DATA* data, int zcode = 255);

// アンビエントの設定
void SetAmbient( int rgb, int zcode = 255 );

// フォグの設定
void SetFog(FOG_DATA *data, int zcode = 255);

//--------------------------------
// その他
//--------------------------------

// ビューマトリクスの取得
void GetViewMatrix(D3DXMATRIX *pMatrix);

// ワールド座標→スクリーン座標変換
void WorldToScreen(float wx, float wy, float wz, int *sx, int *sy);

//==============================================================================
//
//		サウンド
//
//==============================================================================

void Sound();

// 全サウンド制御
void StopSound();
void PauseSound();
void ResumeSound();

// Wave
//void ResizeWave(int n);						// 数の変更
void LoadWave(LOAD_WAVE* pLoad);			// ロード
void ReleaseWave();							// 解放

// SE
void ResizeSE(int n);						// 同時再生数の変更
void PlaySE(WAVE_DATA& wave);
void StopSE();

// BGM
void PlayBGM(WAVE_DATA& wave, bool loop = true);
void StopBGM();
void FadeoutBGM(int fadeTime = 120);	// フェードアウト（2秒）
void TempoupBGM();						// BGMのテンポアップ（1.1倍）

// 音声
void PlayVoice(WAVE_DATA& wave);
void StopVoice();
};

using namespace GL;

//******************************************************************************
#endif // !_GL_H_