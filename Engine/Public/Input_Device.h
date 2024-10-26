#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device : public CBase
{
	
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)			{ return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	
	{ 	return m_tMouseState.rgbButtons[eMouse]; 	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{	
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
	_bool MouseButtonUp(MOUSEKEYSTATE eMouse);

	_bool IsKeyReleased(_ubyte _Key)
	{
		SHORT keyState = GetAsyncKeyState(_Key);
		// Check if the key is currently toggled
		_bool toggled = keyState & 0x8000;

		return toggled;
	}
	_bool isKeyUP(_int _iKey);
	_bool isKeyDown(_int _iKey);
	MOUSEWHEELSTATE CheckMouseWheel(void);
public:
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse	= nullptr;

private:
	_byte					m_byKeyState[256];		// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_bool					m_bKeyState[VK_MAX];
	DIMOUSESTATE			m_tMouseState;	
	
	_bool				    m_bMouseClicked = false;
	_bool				    m_bMouseButtonDown = false;

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
