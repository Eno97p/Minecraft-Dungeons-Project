#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopIcon :  public CShopUI
{
public:
	typedef struct ITEMINFO : public SHOPDESC
	{
		const wchar_t* ItemName;
		const wchar_t* TextureTag;
		_int ItemIndex;
	};
private:
	CShopIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopIcon(const CShopIcon& rhs);
	virtual ~CShopIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	EQUIPSLOTUSAGE Get_Usage() { return m_eType; }
	_int Get_ItemNumber() { return m_ItemNum; }
	
	void Set_Buy(_bool _buyed) { m_Buyed = _buyed; }

	ITEMINFO Get_OwnDesc() { return m_OwnDesc; }
public:
	HRESULT Add_Components(const wchar_t* TextureTag);
	HRESULT Bind_ShaderResources();

private:
	void Set_Rarity();
	void CreateItem_To_Inventory();
	void Sound_No();
private:
	EQUIPSLOTUSAGE m_eType;
	_int			m_ItemNum = 0;
	CGameObject*	m_ItemInfo = nullptr;
	CGameObject*	m_Rarity = nullptr;
	CGameObject*	m_pPrice = nullptr;
	ITEMRARITY		m_eRarity = RARITY_END;
	_bool			m_Buyed = false;
	_int			m_Price = 0;

	ITEMINFO		m_OwnDesc;
public:
	static CShopIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END