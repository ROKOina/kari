//******************************************************************************
//
//
//		Windows関連処理
//
//
//******************************************************************************

// インクルード
#include "GLAll.h"
#include <tchar.h>

static HWND		m_hWnd;
static POINT	m_cursorPos;

static LRESULT CALLBACK MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);	// メッセージ処理

//==============================================================================
//
//		ウインドウ初期設定と終了処理
//
//==============================================================================

// ウインドウの初期設定（登録と作成）
HWND GLWindow::Init( LPCTSTR lpCaption, int width, int height, bool isFullscreen )
{
	//	ウインドウクラスの登録
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), LoadIcon( NULL, IDI_APPLICATION ), LoadCursor( NULL, IDC_ARROW ), (HBRUSH)GetStockObject(BLACK_BRUSH), NULL,
		_T("My Window"), LoadIcon( NULL, IDI_APPLICATION ) };

	RegisterClassEx( &wc );			// ウインドウクラスを登録


	// ウインドウの作成
	m_hWnd = CreateWindow( _T("My Window"), lpCaption,
		( isFullscreen )? WS_POPUP: ( WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX ),
		0, 0, 640, 480,
		NULL, NULL, GetModuleHandle(NULL), NULL );

	// ウインドウの位置とサイズの調整
	RECT	rcWindow;
	RECT	rcClient;
	GetWindowRect(m_hWnd, &rcWindow);				// ウインドウ全体のサイズ取得
	GetClientRect(m_hWnd, &rcClient);				// クライアント領域のサイズ取得

	int w = width  + ( rcWindow.right  - rcWindow.left ) - ( rcClient.right  - rcClient.left );
	int h = height + ( rcWindow.bottom - rcWindow.top  ) - ( rcClient.bottom - rcClient.top  );

	SetWindowPos(m_hWnd, 0, 0, 0, w, h, SWP_NOZORDER);


	ShowWindow(m_hWnd, SW_SHOWDEFAULT);				// ウインドウの外枠の表示
	UpdateWindow(m_hWnd);							// クライアント領域（内側）の更新

	return m_hWnd;
}

//==============================================================================

// ウインドウの終了処理
void GLWindow::Uninit()
{
	UnregisterClass( _T("My Window"), GetModuleHandle(NULL) );
}

//==============================================================================
//
//		メッセージ処理
//
//==============================================================================

// メッセージループ
bool GLWindow::MsgLoop()
{
	MSG		msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
		if (msg.message == WM_QUIT) return false;		// アプリケーション終了
		TranslateMessage(&msg);			// 仮想キーコードをASCIIコードに変換
		DispatchMessage(&msg);			// WndProcへメッセージ配信
	}
	return true;						// メッセージループ終了（ゲーム処理へ）
}

//==============================================================================

// ウインドウプロシージャ
static LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// メッセージの種類に応じて処理を分岐
	switch ( msg ) {
	case WM_KEYDOWN:	// キーが押されたとき
		switch( wParam ) {
		case VK_ESCAPE:		// ESCキーのとき
			PostMessage( hWnd, WM_CLOSE, 0, 0 );	// WM_CLOSE発行
			return 0;
		}
		break;

	case WM_DESTROY:	// ウインドウが破棄されたときの処理
		PostQuitMessage( 0 );
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}

//==============================================================================
//
//		その他
//
//==============================================================================

// カーソル表示
void GLWindow::CursorOn()
{
	while (ShowCursor(TRUE) < 0) {}
}
// カーソル消去
void GLWindow::CursorOff()
{
	while (ShowCursor(FALSE) >= 0) {}
}

//==============================================================================

// カーソル座標の更新
void GLWindow::UpdateCursorPos()
{
	GetCursorPos(&m_cursorPos);
	ScreenToClient(m_hWnd, &m_cursorPos);
}
// カーソル座標（クライアント座標）を返す
POINT GLWindow::GetCursorPos()
{
	return m_cursorPos;
}

//******************************************************************************
