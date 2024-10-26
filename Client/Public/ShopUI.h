#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopUI : public CUI
{
public:
	typedef struct SHOPDESC : public UIDESC
	{
		_float2 In_Pos;
		_float2 In_Size;
	};
protected:
	CShopUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopUI(const CShopUI& rhs);
	virtual ~CShopUI() = default;

protected:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

public:
	void Set_Open(_bool open) {
		S_isOpen = open;
		}
	 
protected:
	static _bool	S_isOpen;

protected:
	CUI* m_Mouse = nullptr;
	_bool isMouseOn = false;
public:
	virtual void Free() override;
};
END
