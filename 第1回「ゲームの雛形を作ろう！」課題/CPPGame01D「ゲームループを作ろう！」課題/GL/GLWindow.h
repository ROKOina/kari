#ifndef _GLWINDOW_H_
#define	_GLWINDOW_H_
//******************************************************************************
//
//
//		Windows関連処理
//
//
//******************************************************************************

#include <Windows.h>

namespace GLWindow
{
	HWND Init(LPCTSTR lpCaption, int width, int height, bool isFullscreen = true);			// ウインドウ初期設定
	void Uninit();				// ウインドウ終了処理
	bool MsgLoop();				// メッセージ処理

	void CursorOn();			// カーソル表示
	void CursorOff();			// カーソル消去
	void UpdateCursorPos();		// カーソル座標の更新
	POINT GetCursorPos();		// カーソル座標（クライアント座標）を返す
};

//******************************************************************************
#endif // !_GLWINDOW_H_