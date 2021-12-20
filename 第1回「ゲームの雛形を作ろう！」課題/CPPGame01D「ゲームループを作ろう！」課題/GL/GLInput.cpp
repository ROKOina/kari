//******************************************************************************
//
//
//		入力関連処理
//
//
//******************************************************************************

#define DIRECTINPUT_VERSION	0x0800

#pragma comment ( lib, "dinput8.lib" )
#pragma comment ( lib, "dxguid.lib" )

//------< include >-------------------------------------------------------------

#include "GLAll.h"
#include <dinput.h>

#include <list>
using namespace std;

//------< extern >--------------------------------------------------------------
//------< define >--------------------------------------------------------------

#define	PAD_MAX			4
#define RANGE_MAX		1000

//------< structure >-----------------------------------------------------------

// GLPadクラス
class GLPad
{
private:
	PAD_ASSIGN*				m_pAssign;
public:
	GLPad(PAD_ASSIGN* pAssign);
	void update(PAD_WORK* pWork);

	virtual void getDeviceState() {}
	virtual int isButtonDown(int button) = 0;
	virtual void getVolume(PAD_WORK* pState) {}
};

// キーボード
class GLKeyboard : public GLPad
{
public:
	GLKeyboard(PAD_ASSIGN* pAssign);
	int isButtonDown(int button);
};

// ジョイスティック
class GLJoystick : public GLPad
{
private:
	LPDIRECTINPUTDEVICE8	m_pDIDevice;
	DIJOYSTATE2				m_state;
public:
	GLJoystick(PAD_ASSIGN* pAssign, LPDIRECTINPUTDEVICE8 pDIDevice);
	void getDeviceState();
	int isButtonDown(int button);
	void getVolume(PAD_WORK* pState);
};

//------< data >----------------------------------------------------------------

//--------------------------------
// ビット割り当てデータ
//--------------------------------

// キーボード用データ1P
static DEVICE_ASSIGN keyAssign00 = {
	GLKEYBOARD,
	{
		{ PAD_UP   , VK_UP },
		{ PAD_DOWN , VK_DOWN },
		{ PAD_LEFT , VK_LEFT },
		{ PAD_RIGHT, VK_RIGHT },
		{ PAD_START, VK_SPACE },
		{ PAD_TRG1 , 'Z' },
		{ PAD_TRG2 , 'X' },
		{ PAD_TRG3 , 'C' },
		{ PAD_SELECT, VK_F1 },
		// マウス
		//{ PAD_TRG1 , VK_LBUTTON },
		//{ PAD_TRG2 , VK_RBUTTON },
		//{ PAD_START, VK_MBUTTON },
		{ 0x00, 0x00 }				// 終了コード
	}
};
// ジョイスティック用データ
static DEVICE_ASSIGN joyAssign00 = {
	GLJOYSTICK,
	{
		{ PAD_UP    , JOY_BUTTON_13 },
		{ PAD_RIGHT , JOY_BUTTON_14 },
		{ PAD_DOWN  , JOY_BUTTON_15 },
		{ PAD_LEFT  , JOY_BUTTON_16 },
		{ PAD_START , JOY_BUTTON_10 },
		{ PAD_TRG1  , JOY_BUTTON_01 },
		{ PAD_TRG2  , JOY_BUTTON_02 },
		{ PAD_TRG3  , JOY_BUTTON_03 },
		{ PAD_SELECT, JOY_BUTTON_09 },
		{ 0x00, 0x00 }				// 終了コード
	}
};

//--------------------------------
// デバイス割り当てデータ
//--------------------------------

// 1P用デバイスリスト
static DEVICE_ASSIGN* device_1p[] = {
	&joyAssign00,
	&keyAssign00,
	nullptr,
};
// 2P用デバイスリスト
static DEVICE_ASSIGN* device_2p[] = {
	&joyAssign00,
	nullptr,
};
// 3P用デバイスリスト
static DEVICE_ASSIGN* device_3p[] = {
	&joyAssign00,
	nullptr,
};
// 4P用デバイスリスト
static DEVICE_ASSIGN* device_4p[] = {
	&joyAssign00,
	nullptr,
};

static DEVICE_ASSIGN** device_default[] = {
	device_1p,
	device_2p,
	device_3p,
	device_4p,
};

//------< work >----------------------------------------------------------------

static LPDIRECTINPUT8				m_pDI;
static list<LPDIRECTINPUTDEVICE8>	m_JoystickList;
static GLPad*						m_pGLPad[PAD_MAX];
static PAD_WORK						m_pad_work[PAD_MAX];

//------< prototype >-----------------------------------------------------------

// コールバック
static BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext);
static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

static void CreatePad(DEVICE_ASSIGN*** pppDevice);
static void DeletePad();

//==============================================================================
//
//		GLInput
//
//==============================================================================

// 初期設定
HRESULT GLInput::Init(HWND hWnd)
{
	// DirectInputの初期化
	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDI, NULL))) return E_FAIL;

	// ジョイスティックを探す
	m_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyCallback, 0, DIEDFL_ATTACHEDONLY);

	// ジョイスティックの設定
	for (auto& pDIDJoystick : m_JoystickList) {
		pDIDJoystick->SetDataFormat(&c_dfDIJoystick2);									// データフォーマットの設定
		pDIDJoystick->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);	// 協調レベルの設定
		pDIDJoystick->EnumObjects(EnumAxesCallback, pDIDJoystick, DIDFT_AXIS);			// 「軸」を探す
	}

	// GLPadの生成
	CreatePad(device_default);

	// 入力ワーククリア
	ZeroMemory(m_pad_work, sizeof(m_pad_work));

	return S_OK;
}

//------------------------------------------------------------------------------

// ジョイスティックデバイスの作成（ジョイスティックが見つかると呼び出される）
static BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE *pdidInstance, VOID *pContext)
{
	LPDIRECTINPUTDEVICE8	pDIDJoystick;
	if (SUCCEEDED(m_pDI->CreateDevice(pdidInstance->guidInstance, &pDIDJoystick, NULL))) {
		m_JoystickList.push_back(pDIDJoystick);
	}
	return DIENUM_CONTINUE;
}

//------------------------------------------------------------------------------

// 「軸」の設定（「軸」が見つかるごとに呼び出される）
static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	DIPROPRANGE	diprg;		// ジョイスティック入力データ設定用構造体
	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwHow = DIPH_BYID;					// IDで指定
	diprg.diph.dwObj = lpddoi->dwType;				// 軸のID
	diprg.lMin = -RANGE_MAX;						// 軸の最小値
	diprg.lMax =  RANGE_MAX;						// 軸の最大値

	((LPDIRECTINPUTDEVICE8)pvRef)->SetProperty(DIPROP_RANGE, &diprg.diph);
	return DIENUM_CONTINUE;
}

//==============================================================================

// 終了処理
void GLInput::Uninit()
{
	// GLPadの削除
	DeletePad();

	// ジョイスティックの削除
	for (auto& pDIDJoystick : m_JoystickList) {
		pDIDJoystick->Unacquire();
		pDIDJoystick->Release();
	}

	// DirectInputの削除
	m_pDI->Release();
}

//==============================================================================

// 更新処理
void GLInput::Update()
{
	// Padデータの更新
	for (int i = 0; i < PAD_MAX; i++) {
		if (m_pGLPad[i]) m_pGLPad[i]->update(&m_pad_work[i]);
	}
}

//==============================================================================

// 入力デバイスの設定
void GLInput::SetDevice(DEVICE_ASSIGN*** pppDevice)
{
	DeletePad();
	CreatePad(pppDevice);
}

//==============================================================================

// 入力ワークのアドレスを返す
PAD_WORK* GLInput::GetPadWork(int i)
{
	return &m_pad_work[i];
}

//==============================================================================

// GLPadの生成
static void CreatePad(DEVICE_ASSIGN*** pppDevice)
{
	auto ppDIDJoystick = m_JoystickList.begin();

	for (int i = 0; i < PAD_MAX; i++) {
		GLPad* pad = nullptr;
		for (DEVICE_ASSIGN** ppDevice = pppDevice[i]; *ppDevice; ppDevice++) {
			DEVICE_ASSIGN* pDevice = *ppDevice;
			switch (pDevice->device) {
			case GLKEYBOARD:
				pad = new GLKeyboard(pDevice->assign);
				break;
			case GLJOYSTICK:
				if (ppDIDJoystick != m_JoystickList.end()) {
					pad = new GLJoystick(pDevice->assign, *ppDIDJoystick);
					ppDIDJoystick++;
				}
				break;
			}
			if (pad) break;
		}
		m_pGLPad[i] = pad;
	}
}

//==============================================================================

// GLPadの削除
static void DeletePad()
{
	for (auto& pad : m_pGLPad) {
		if (pad) {
			delete pad;
			pad = nullptr;
		}
	}
}

//==============================================================================
//
//		GLPadクラス
//
//==============================================================================

// GLPadクラス（基底クラス）
GLPad::GLPad(PAD_ASSIGN* pAssign) : m_pAssign(pAssign)
{
}

void GLPad::update(PAD_WORK* pWork)
{
	// 入力データ取得
	getDeviceState();

	int	old_state = pWork->state;
	pWork->state = 0;

	// 入力更新
	PAD_ASSIGN* pAssign = m_pAssign;
	if (pAssign) {
		while (pAssign->bit) {
			if (isButtonDown(pAssign->code)) pWork->state |= pAssign->bit;
			pAssign++;
		}
	}

	// トリガー入力
	pWork->trg = (~old_state) & pWork->state;

	// アナログ入力
	getVolume(pWork);
}

//==============================================================================

// GLKeyboardクラス
GLKeyboard::GLKeyboard(PAD_ASSIGN* pAssign) : GLPad(pAssign)
{
}

int GLKeyboard::isButtonDown(int button)
{
	return (GetKeyState(button) < 0);
}

//==============================================================================

// GLJoystickクラス
GLJoystick::GLJoystick(PAD_ASSIGN* pAssign, LPDIRECTINPUTDEVICE8 pDIDevice) : GLPad(pAssign), m_pDIDevice(pDIDevice)
{
}

void GLJoystick::getDeviceState()
{
	if (SUCCEEDED(m_pDIDevice->Poll())) {
		m_pDIDevice->GetDeviceState(sizeof(DIJOYSTATE2), &m_state);
	}
	else {
		m_pDIDevice->Acquire();
	}
}

int GLJoystick::isButtonDown(int button)
{
	return (m_state.rgbButtons[button] & 0x80);
}

void GLJoystick::getVolume(PAD_WORK* pWork)
{
	pWork->volume[0] = (float)m_state.lX / RANGE_MAX;
	pWork->volume[1] = (float)m_state.lY / RANGE_MAX;
	pWork->volume[2] = (float)m_state.lZ / RANGE_MAX;
	pWork->volume[3] = (float)m_state.lRz / RANGE_MAX;
}

//******************************************************************************