#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CInvenUI : public CUI
{
public:
	typedef struct INVENDESC : public UIDESC
	{
		_float2 In_Pos;
		_float2 In_Size;
	};
protected:
	CInvenUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInvenUI(const CInvenUI& rhs);
	virtual ~CInvenUI() = default;

protected:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void Set_Open(_bool open) {
		isOpen = open;
		}
	void Set_AllClose(_bool close)
	{
		AllClose = close;
	}
	 
protected:
	static _bool	isOpen;
	static _bool	AllClose;

protected:
	CUI* m_Mouse = nullptr;
	_bool isMouseOn = false;
public:
	virtual void Free() override;
};
END
