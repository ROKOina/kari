//******************************************************************************
//
//
//		ライブラリインターフェイス
//
//
//******************************************************************************
//------< include >-------------------------------------------------------------

#include "GL.h"
using namespace GL;

#include "GLAll.h"

//******************************************************************************
//
//
//		GLインターフェイス
//
//
//******************************************************************************
//==============================================================================
//
//		ライブラリ初期設定＆終了処理、Windows APIなど
//
//==============================================================================

// ライブラリ初期設定
void GL::Init( LPCTSTR lpCaption, int width, int height, bool isFullscreen )
{
	HWND hWnd = GLWindow::Init(lpCaption, width, height, isFullscreen);
	if (isFullscreen) GLWindow::CursorOff();	// フルスクリーン時カーソルOFF

	GLDraw::Init(hWnd, width, height, isFullscreen);
	GLInput::Init(hWnd);
	GLSound::Init();
}

// ライブラリ終了処理
void GL::End()
{
	GLSound::Uninit();
	GLInput::Uninit();
	GLDraw::Uninit();
	GLWindow::Uninit();
}

// ゲームループ制御
bool GL::GameLoop() {
	GLSound::Update();
	return GLWindow::MsgLoop();
}

// カーソルON/OFF
void GL::CursorOn() { GLWindow::CursorOn(); }
void GL::CursorOff() { GLWindow::CursorOff(); }

//==============================================================================
//
//		入力処理
//
//==============================================================================

// 入力デバイスの設定
void GL::SetDevice(DEVICE_ASSIGN*** pppDevice) { GLInput::SetDevice(pppDevice); }

// 入力情報の更新
void GL::Input()
{
	GLInput::Update();
	GLWindow::UpdateCursorPos();
}

// 入力ワークのアドレス取得
PAD_WORK *GL::GetPadWork(int i) { return GLInput::GetPadWork(i); }

// カーソル座標の取得
int GL::GetCursorPosX() { return (int)GLWindow::GetCursorPos().x; }
int GL::GetCursorPosY() { return (int)GLWindow::GetCursorPos().y; }

//==============================================================================
//
//		描画制御
//
//==============================================================================

void GL::BeginScene() { GLDraw::BegineScene(); }
void GL::ClearScene( int argb ) { GLDraw::ClearScene(argb); }
void GL::EndScene() { GLDraw::EndScene(); }

//==============================================================================
//
//		リソース管理
//
//==============================================================================

// テクスチャ
void GL::LoadTexture(LOAD_TEXTURE* pLoad) { GLDraw::LoadTexture(pLoad); }
void GL::ReleaseTexture() { GLDraw::ReleaseTexture(); }

// フォント
void GL::ResizeFont(int n) { GLDraw::ResizeFont(n); }
void GL::LoadFont(LOAD_FONT* pLoad) { GLDraw::LoadFont(pLoad); }
void GL::ReleaseFont() { GLDraw::ReleaseFont(); }

// メッシュ
void GL::ResizeMesh(int n) { GLDraw::ResizeMesh(n); }
void GL::LoadMesh(LOAD_MESH* pLoad) { GLDraw::LoadMesh(pLoad); }
void GL::ReleaseMesh() { GLDraw::ReleaseMesh(); }

//==============================================================================
//
//		描画コマンドの追加
//
//==============================================================================

// スプライト描画
void GL::DrawSprite(float x, float y, SPRITE_DATA* data, float sx, float sy, float rot, bool xflip, bool yflip, float sh, float sv, int db, int alpha, int tb, int r, int g, int b, int zcode)
{
	GLDraw::DrawSprite(x, y, data, sx, sy, rot, xflip, yflip, sh, sv, db, tb, D3DCOLOR_ARGB(alpha, r, g, b), zcode);
}

// 文字列描画
void GL::DrawString(int x, int y, STRING_DATA* data, int db, int alpha, int count, int zcode)
{
	GLDraw::DrawString(x, y, data->fontNum, data->str, data->color, data->isCenter, db, alpha, count, zcode);
}
void GL::DrawString(int x, int y, int fontNum, const char* str, int color, bool isCenter, int db, int alpha,int count, int zcode)
{
	GLDraw::DrawString(x, y, fontNum, str, color, isCenter, db, alpha, count, zcode);
}

// 直線描画
void GL::DrawLine(float x0, float y0, float x1, float y1, int color, int db, int alpha, int zcode)
{
	GLDraw::DrawLine(x0, y0, x1, y1, color, db, alpha, zcode);
}

// 四角形描画
void GL::DrawRectangle(float left, float top, float width, float height, int color, int db, int alpha, int zcode)
{
	GLDraw::DrawRectangle(left, top, width, height, color, db, alpha, zcode);
}

// メッシュ描画
void GL::DrawMesh(MESH_DATA* data, D3DXMATRIX& matTrans, int db, float alpha, float r, float g, float b, int zcode)
{
	GLDraw::DrawMesh(data, matTrans, db, alpha, r, g, b, zcode);
}

//==============================================================================
//
//		描画設定の追加
//
//==============================================================================

void GL::SetCamera(CAMERA_DATA* data, int zcode) { GLDraw::SetCamera(data, zcode); }	// カメラ
void GL::SetLight(LIGHT_DATA* data, int zcode) { GLDraw::SetLight(data, zcode); }		// ライト
void GL::SetAmbient( int rgb, int zcode ) { GLDraw::SetAmbient(rgb, zcode); }			// アンビエント
void GL::SetFog(FOG_DATA* data, int zcode) { GLDraw::SetFog(data, zcode); }				// フォグ

//==============================================================================
//
//		その他
//
//==============================================================================

// ビューマトリクスの取得
void GL::GetViewMatrix(D3DXMATRIX *pMatrix) { GLDraw::GetViewMatrix(pMatrix); }

// ワールド座標→スクリーン座標変換
void GL::WorldToScreen(float wx, float wy, float wz, int *sx, int *sy) { GLDraw::WorldToScreen(wx, wy, wz, sx, sy); }

//==============================================================================
//
//		サウンド
//
//==============================================================================

void GL::Sound() { GLSound::Update(); }
// サウンド制御
void GL::StopSound() { GLSound::StopSound(); }
void GL::PauseSound() { GLSound::PauseSound(); }
void GL::ResumeSound() { GLSound::ResumeSound(); }

// Waveファイル
//void GL::ResizeWave(int n) { GLSound::ResizeWave(n); }
void GL::LoadWave(LOAD_WAVE* pLoad) { GLSound::LoadWave(pLoad); }
void GL::ReleaseWave() { GLSound::ReleaseWave(); }

// SE
void GL::ResizeSE(int n) { GLSound::ResizeSE(n); }
void GL::PlaySE(WAVE_DATA& wave) { GLSound::PlaySE(wave); }
void GL::StopSE() { GLSound::StopSE(); }

// BGM
void GL::PlayBGM(WAVE_DATA& wave, bool loop) { GLSound::PlayBGM(wave, loop); }
void GL::StopBGM() { GLSound::StopBGM(); }
void GL::FadeoutBGM(int fadeTime) { GLSound::FadeoutBGM(fadeTime); }
void GL::TempoupBGM() { GLSound::TempoupBGM(); }

// 音声
void GL::PlayVoice(WAVE_DATA& wave) { GLSound::PlayVoice(wave); }
void GL::StopVoice() { GLSound::StopVoice(); }

//******************************************************************************
