#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShop_Info :  public CShopUI
{
public:
	typedef struct ITEMINFOINFO : public SHOPDESC
	{
		_int iItemIndex;
		const wchar_t* ItemName;
		ITEMRARITY		eRarity;
	};
private:
	CShop_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShop_Info(const CShop_Info& rhs);
	virtual ~CShop_Info() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Add_Components();
private:
	void Set_Info_Font();

private:
	const wchar_t* m_ItemName;
	const wchar_t* m_ItemInfo;
	const wchar_t* m_ItemInfo2;
	_int m_Itemindex = 0;
	_float4 NameColor = {};
public:
	static CShop_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END