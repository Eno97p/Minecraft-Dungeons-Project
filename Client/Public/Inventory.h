#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"
BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CInventory :  public CBase
{
	DECLARE_SINGLETON(CInventory)
public:
	enum INVENTYPE { FRAME, SLOT, EQUIP_SLOT, ITEM, EQUIP_ITEM, INFO, EMERALD_INFO, HOTBARSLOT, INVEN_END};
	enum SHOPTYPE { S_FRAME, S_SLOT, S_ITEM , S_INFO, S_BUTTON, SHOP_END };
	
public:
	CInventory();
	virtual ~CInventory() = default;
public:
	HRESULT Initialize(CGameObject* pPlayer);
	void Tick(_float fTimeDelta);

	void Add_Emerald() { m_NumEmerald++; }
	void Minus_Emerald(_int iPrice) { m_NumEmerald -= iPrice; }
	_int* Get_NumEmerald() {
		 return &m_NumEmerald;
	}
	HRESULT Add_Inven_Item(class CItem* pItem);
	HRESULT Buy_Shop_Item(class CShopIcon* pIcon);
	HRESULT Swap_Slot(class CInven_ItemIcon* pItemIcon);
	_bool* Get_InvenOpen() { return &m_bShowInven; }
	_bool* Get_ShopOpen() { return &m_bShowShop; }
private:
	void Add_Inven_Value();
	void Add_Shop_Value();

private:
	void Clear();
	void Render();

	void Sound_Yes();
	void Sound_No();


private:
	CGameInstance* m_pGameInstance = nullptr;
	CPlayer* m_pPlayer = nullptr;
	CGameObject* m_pDummyplayer = nullptr;
	class CMouse* m_Mouse = nullptr;
	CPlayer::PLAYERSTATUS* m_pPlayerStatus = nullptr;
	class CFreeCamera* m_pCamera = nullptr;
private:
	//vector<class CInvenUI*> m_InvenValues[INVEN_END];
	class CInvenUI* m_AInvenValues[INVEN_END][12];

	//vector<class CShopUI*> m_ShopValues[SHOP_END];
	class CShopUI* m_AShopValues[SHOP_END][6];


private:
	_bool m_bShowInven = false;
	_bool m_bShowShop = false;
	_bool m_bActivate = false;
	_int	m_NumEmerald;
public:
	virtual void Free() override;
};

END