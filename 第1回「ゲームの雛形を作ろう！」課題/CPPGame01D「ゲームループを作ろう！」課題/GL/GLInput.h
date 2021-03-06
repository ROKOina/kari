#ifndef _GLINPUT_H_
#define	_GLINPUT_H_
//******************************************************************************
//
//
//		入力関連処理
//
//
//******************************************************************************

namespace GLInput
{
	HRESULT Init(HWND hWnd);
	void Uninit();
	void Update();

	void SetDevice(DEVICE_ASSIGN*** pppDevice);
	PAD_WORK* GetPadWork(int i = 0);
};

//******************************************************************************
#endif // !_GLINPUT_H_