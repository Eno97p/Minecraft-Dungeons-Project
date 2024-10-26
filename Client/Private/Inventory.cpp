#include "stdafx.h"
#include "Inventory.h"
#include "InvenUI.h"
#include "Inven_ItemIcon.h"
#include "Inven_Frame.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Player.h"
#include "Mouse.h"
#include "Emerald_UI.h"
#include "Item.h"
#include "DummyPlayer.h"
#include "HPBar.h"
#include "HotBarIcon.h"
#include "FreeCamera.h"


#pragma region SHOP
#include "Shop_Frame.h"
#include "ShopUI.h"
#include "Shop_Icon.h"
#pragma endregion SHOP

#include "HUD.h"


IMPLEMENT_SINGLETON(CInventory)

CInventory::CInventory()
{
}

HRESULT CInventory::Initialize(CGameObject* pPlayer)
{
	m_pGameInstance = CGameInstance::GetInstance();

	for (int i = 0; i < INVEN_END; ++i)
	{
		for (int j = 0; j < 12; ++j)
		{
			m_AInvenValues[i][j] = nullptr;
		}
	}

	m_pPlayer = static_cast<CPlayer*>(pPlayer);
	m_Mouse = static_cast<CMouse*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Mouse"), "Mouse"));
	m_NumEmerald = 0;
	m_pPlayerStatus = m_pPlayer->Get_Status();
	Add_Inven_Value();
	Add_Shop_Value();

	
	m_AInvenValues[FRAME][0]->Set_Open(m_bShowInven);
	m_pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));
	m_bActivate = true;

	CHUD::HUDDESC hudesc{};
	hudesc.pPlayerStat = m_pPlayerStatus;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_HUD"), &hudesc);
	return S_OK;
}

void CInventory::Tick(_float fTimeDelta)
{
	if (!m_bActivate)
		return;


	if (m_pGameInstance->isKeyDown('I'))
	{
		m_bShowInven = !m_bShowInven;
		m_pGameInstance->PlaySound_Z(_T("sfx_ui_inventoryArmoryOpen-001_soundWave.ogg"), SOUND_UI, 0.3f);
		m_AInvenValues[INVENTYPE::FRAME][0]->Set_Open(m_bShowInven);
	}


	if (m_pGameInstance->isKeyDown('F'))
	{
	
		if (m_bShowShop)
		{
			
			m_pCamera->Reset_Distance();
			m_pCamera->Set_Target(m_pPlayer);
			m_bShowShop = false;
		}
		else
		{
			
			m_pCamera->Set_Shop();
			m_bShowShop = true;
		}
		m_pGameInstance->PlaySound_Z(_T("sfx_ui_inventoryArmoryOpen-001_soundWave.ogg"), SOUND_UI, 0.3f);
		m_AShopValues[SHOPTYPE::S_FRAME][0]->Set_Open(m_bShowShop);
		m_AInvenValues[INVENTYPE::FRAME][0]->Set_AllClose(m_bShowShop);
	}


}

void CInventory::Add_Inven_Value()
{
	CGameObject* Invenvalue = nullptr;
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_InvenFrame"), &Invenvalue);
	m_AInvenValues[INVENTYPE::FRAME][0] = static_cast<CInvenUI*>(Invenvalue);

	static_cast<CInvenUI*>(Invenvalue)->Set_Open(false);

	
#pragma region ADDSLOT
	//Slot
	CInvenUI::INVENDESC desc{};
	desc.In_Size = _float2(100.f, 100.f);
	int startX = 500;
	int stepX = 110;
	// 세로 시작점과 간격
	int startY = 200;
	int stepY = 110;

	int NumSize = 0;
	for (int y = startY; y <= startY + stepY * 3; y += stepY) {
		for (int x = startX; x <= startX + stepX * 2; x += stepX) {
			// 현재 좌표 출력
			desc.In_Pos = _float2(x, y);
			m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_InvenMainSlot"), &Invenvalue, &desc);
			m_AInvenValues[INVENTYPE::SLOT][NumSize] = static_cast<CInvenUI*>(Invenvalue);
			NumSize++;
		}
	}
	


	desc.In_Size = _float2(70.f, 70.f);
	desc.In_Pos = _float2(90.f, 170.f);
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_InvenMainSlot"), &Invenvalue, &desc);
	m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIPSLOTUSAGE::EQUIP_WEAPON] = static_cast<CInvenUI*>(Invenvalue);

	/*_float4 position_value = m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIP_WEAPON]->Get_Position_Value();*/


	desc.In_Pos = _float2(200.f, 150.f);
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_InvenMainSlot"), &Invenvalue, &desc);
	m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIPSLOTUSAGE::EQUIP_ARMOR] = static_cast<CInvenUI*>(Invenvalue);

	desc.In_Pos = _float2(310.f, 170.f);
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_InvenMainSlot"), &Invenvalue, &desc);
	m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIPSLOTUSAGE::EQUIP_BOW] = static_cast<CInvenUI*>(Invenvalue);

	desc.In_Pos = _float2(1150.f, 30.f);
	desc.In_Size = _float2(30.f, 30.f);
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Emerald_UI"), &Invenvalue, &desc);
	m_AInvenValues[INVENTYPE::EMERALD_INFO][0] = static_cast<CInvenUI*>(Invenvalue);


	_float4 position_value = m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIPSLOTUSAGE::EQUIP_WEAPON]->Get_Position_Value();
	CInven_ItemIcon::ITEMINFO infodesc{};

	infodesc.ItemIndex = 10;
	infodesc.ItemName = TEXT("Sword");
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_Sword");
	infodesc.In_Pos = _float2(position_value.x, position_value.y);
	infodesc.In_Size = _float2(position_value.z, position_value.w);

	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Inven_Icon"), &Invenvalue, &infodesc);
	m_AInvenValues[INVENTYPE::EQUIP_ITEM][EQUIPSLOTUSAGE::EQUIP_WEAPON] = static_cast<CInvenUI*>(Invenvalue);

	position_value = m_AInvenValues[INVENTYPE::EQUIP_SLOT][EQUIPSLOTUSAGE::EQUIP_BOW]->Get_Position_Value();
	infodesc.ItemIndex = 11;
	infodesc.ItemName = TEXT("Bow");
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_Bow");
	infodesc.In_Pos = _float2(position_value.x, position_value.y);
	infodesc.In_Size = _float2(position_value.z, position_value.w);

	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Inven_Icon"), &Invenvalue, &infodesc);
	m_AInvenValues[INVENTYPE::EQUIP_ITEM][EQUIPSLOTUSAGE::EQUIP_BOW] = static_cast<CInvenUI*>(Invenvalue);

#pragma endregion ADDSLOT

	CGameObject::GAMEOBJECT_DESC gojdesc{};
	gojdesc.fRotationPerSec = XMConvertToRadians(90.f);
	gojdesc.fSpeedPerSec = 20.f;

	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_DummyPlayer"), TEXT("Prototype_GameObject_Dummyplayer"), &m_pDummyplayer, &gojdesc);

	static_cast<CDummyPlayer*>(m_pDummyplayer)->SetPlayer(m_pPlayer);

	desc.In_Pos = _float2((g_iWinSizeX * 0.5f), g_iWinSizeY - 35.f);
	desc.In_Size = _float2((g_iWinSizeX * 0.7f), 70.f);

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarFrame"), &desc);

	desc.In_Pos = _float2((g_iWinSizeX * 0.5f), g_iWinSizeY - 70.f);
	desc.In_Size = _float2(150.f, 150.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HPBarFrame"), &desc);


	CHPBar::HPBARDESC bar = {};
	bar.In_Pos = _float2((g_iWinSizeX * 0.5f), g_iWinSizeY - 70.f);
	bar.In_Size = _float2(125.f, 115.f);
	bar.pPlayerStat = m_pPlayer->Get_Status();
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HPBar"), &bar);


	desc.In_Pos = _float2((g_iWinSizeX * 0.674f), g_iWinSizeY - 55.f);
	desc.In_Size = _float2(50.f, 35.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_DashIcon"), &desc);

	desc.In_Pos = _float2(300.f, g_iWinSizeY - 55.f);	//맨 왼쪽부터
	desc.In_Size = _float2(70.f, 70.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarSlot"), &desc);

	desc.In_Pos = _float2(395.f, g_iWinSizeY - 55.f);
	desc.In_Size = _float2(90.f, 90.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarSlot"), &desc);

	desc.In_Pos = _float2(500.f, g_iWinSizeY - 55.f);
	desc.In_Size = _float2(90.f, 90.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarSlot"), &desc);

	desc.In_Pos = _float2(760.f, g_iWinSizeY - 55.f);
	desc.In_Size = _float2(80.f, 80.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarSlot"), &desc);

	desc.In_Pos = _float2(950.f, g_iWinSizeY - 55.f);
	desc.In_Size = _float2(90.f, 110.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_Arrow_Slot"), &desc);




	CHotBarIcon::HOTBARICON hotbardesc{};	//맨 왼쪽
	hotbardesc.In_Pos = _float2(300.f, g_iWinSizeY - 55.f);
	hotbardesc.In_Size = _float2(55.f, 55.f);
	hotbardesc.Font = TEXT("[I]");
	hotbardesc.TextureTag = TEXT("Prototype_Component_Texture_InvenIcon");
	hotbardesc.FontPosition = _float2(6.f, -1.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarIcon"), &hotbardesc);




	hotbardesc.In_Pos = _float2(760.f, g_iWinSizeY - 55.f);	//HP포션 아이콘
	hotbardesc.In_Size = _float2(60.f, 60.f);
	hotbardesc.Font = TEXT("[E]");
	hotbardesc.Value = &(m_pPlayerStatus->iNumPotion);
	hotbardesc.TextureTag = TEXT("Prototype_Component_Texture_HP_Potion");
	hotbardesc.FontPosition = _float2(6.f, 0.f);
	hotbardesc.Font2Position = _float2(-40.f, 0.f);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarIcon"), &hotbardesc);



	//화살통
	hotbardesc.In_Pos = _float2(950.f, g_iWinSizeY - 55.f);
	hotbardesc.In_Size = _float2(70.f, 70.f);
	hotbardesc.Font = TEXT("RBT");
	hotbardesc.Value = &(m_pPlayerStatus->iNumArrows);
	hotbardesc.TextureTag = TEXT("Prototype_Component_Texture_ArrowIcon");
	hotbardesc.FontPosition = _float2(6.f, 0.f);

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_HotBar"), TEXT("Prototype_GameObject_HotBarIcon"), &hotbardesc);




}

void CInventory::Add_Shop_Value()
{
	CGameObject* ShopValue = nullptr;
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Frame"), &ShopValue);
	m_AShopValues[SHOPTYPE::S_FRAME][0] = static_cast<CShopUI*>(ShopValue);
	
	static_cast<CShopUI*>(ShopValue)->Set_Open(false);

	CShopUI::SHOPDESC desc{};
	desc.In_Size = _float2(150.f, 150.f);
	int startX = 780;
	int startY = 150;
	// 상자 간의 간격
	int deltaX = 160;
	int deltaY = 160;
	_int Numsize = 0;
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			int x = startX + j * deltaX;
			int y = startY + i * deltaY;
			desc.In_Pos = _float2(x, y);
			m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Slot"), &ShopValue, &desc);
			m_AShopValues[SHOPTYPE::S_SLOT][Numsize] = static_cast<CShopUI*>(ShopValue);
			Numsize++;
		}
	}


	CShopIcon::ITEMINFO infodesc{};
	_float4 PosVal = m_AShopValues[SHOPTYPE::S_SLOT][0]->Get_Position_Value();
	infodesc.In_Pos = _float2(PosVal.x, PosVal.y);
	infodesc.In_Size = _float2(PosVal.z - 10.f, PosVal.w - 10.f);
	infodesc.ItemIndex = 1;
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_Potion");
	infodesc.ItemName = TEXT("Potion");
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Icon"), &ShopValue, &infodesc);
	m_AShopValues[SHOPTYPE::S_ITEM][0] = static_cast<CShopUI*>(ShopValue);


	PosVal = m_AShopValues[SHOPTYPE::S_SLOT][1]->Get_Position_Value();
	infodesc.In_Pos = _float2(PosVal.x, PosVal.y);
	infodesc.ItemIndex = 12;
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_ArrowIcon");
	infodesc.ItemName = TEXT("Arrows");
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Icon"), &ShopValue, &infodesc);
	m_AShopValues[SHOPTYPE::S_ITEM][1] = static_cast<CShopUI*>(ShopValue);


	PosVal = m_AShopValues[SHOPTYPE::S_SLOT][2]->Get_Position_Value();
	infodesc.In_Pos = _float2(PosVal.x, PosVal.y);
	infodesc.ItemIndex = 4;
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_Claymore");
	infodesc.ItemName = TEXT("Claymore");
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Icon"), &ShopValue, &infodesc);
	m_AShopValues[SHOPTYPE::S_ITEM][2] = static_cast<CShopUI*>(ShopValue);

	PosVal = m_AShopValues[SHOPTYPE::S_SLOT][3]->Get_Position_Value();
	infodesc.In_Pos = _float2(PosVal.x, PosVal.y);
	infodesc.ItemIndex = 8;
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_MercenaryArmor");
	infodesc.ItemName = TEXT("Mercenary Armor");
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Icon"), &ShopValue, &infodesc);
	m_AShopValues[SHOPTYPE::S_ITEM][3] = static_cast<CShopUI*>(ShopValue);

	PosVal = m_AShopValues[SHOPTYPE::S_SLOT][4]->Get_Position_Value();
	infodesc.In_Pos = _float2(PosVal.x, PosVal.y);
	infodesc.ItemIndex = 7;
	infodesc.TextureTag = TEXT("Prototype_Component_Texture_Item_Spear");
	infodesc.ItemName = TEXT("Spear");
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Shop"), TEXT("Prototype_GameObject_Shop_Icon"), &ShopValue, &infodesc);
	m_AShopValues[SHOPTYPE::S_ITEM][4] = static_cast<CShopUI*>(ShopValue);


}

HRESULT CInventory::Add_Inven_Item(CItem* pItem)
{
	CGameObject* Invenvalue = nullptr;

	CInven_ItemIcon::ITEMINFO infodesc{};
	infodesc.ItemIndex = pItem->Get_ItemDesc().RandomValue;
	if (infodesc.ItemIndex == 0)// 여기는 포션먹었을때
	{
		m_pPlayer->Swift();
		//25초
		//스위프트 포션 효과
		return S_OK; 
	}
	else if (infodesc.ItemIndex == 1)
	{
		m_pPlayerStatus->iNumPotion += 1;
		return S_OK;
	}
	else if (infodesc.ItemIndex == 12)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_item_arrowDrop-001_soundWave.ogg"), SOUND_EFFECT, 0.3f);
		m_pPlayerStatus->iNumArrows += 20;
		return S_OK;
	}

	infodesc.ItemName = pItem->Get_ItemDesc().Item_Name;
	infodesc.TextureTag = pItem->Get_ItemDesc().TextureTag;
	
	_int index = 0;
	for (int i = 0; i < 12; ++i)
	{
		if (m_AInvenValues[INVENTYPE::ITEM][i] == nullptr)
		{
			index = i;
			break;
		}

		if (i == 11)
		{
			MSG_BOX("Out Of Size Inventory");
			return S_OK;
		}
	}


	_float4 position_value = m_AInvenValues[INVENTYPE::SLOT][index]->Get_Position_Value();

	infodesc.In_Pos = _float2(position_value.x, position_value.y);
	infodesc.In_Size = _float2(position_value.z, position_value.w);

	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Inven_Icon"), &Invenvalue, &infodesc);
	m_AInvenValues[INVENTYPE::ITEM][index] = static_cast<CInvenUI*>(Invenvalue);
	return S_OK;
}

HRESULT CInventory::Buy_Shop_Item(CShopIcon* pIcon)
{
	CShopIcon::ITEMINFO icon{};
	icon = pIcon->Get_OwnDesc();

	CInven_ItemIcon::ITEMINFO infodesc{};
	infodesc.ItemIndex = icon.ItemIndex;
	infodesc.TextureTag = icon.TextureTag;
	infodesc.ItemName = icon.ItemName;

	if (infodesc.ItemIndex == 1)
	{
		m_pPlayerStatus->iNumPotion += 1;
		Sound_Yes();
		return S_OK;
	}
	else if (infodesc.ItemIndex == 12)
	{
		m_pPlayerStatus->iNumArrows += 20;
		Sound_Yes();
		return S_OK;
	}


	_int index = 0;
	for (int i = 0; i < 12; ++i)
	{
		if (m_AInvenValues[INVENTYPE::ITEM][i] == nullptr)
		{
			index = i;
			break;
		}

		if (i == 11)
		{
			MSG_BOX("Out Of Size Inventory");
			return S_OK;
		}
	}
	_float4 position_value = m_AInvenValues[INVENTYPE::SLOT][index]->Get_Position_Value();
	infodesc.In_Pos = _float2(position_value.x, position_value.y);
	infodesc.In_Size = _float2(position_value.z, position_value.w);

	CGameObject* Invenvalue = nullptr;
	m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, TEXT("Layer_Inven"), TEXT("Prototype_GameObject_Inven_Icon"), &Invenvalue, &infodesc);
	m_AInvenValues[INVENTYPE::ITEM][index] = static_cast<CInvenUI*>(Invenvalue);
	m_pGameInstance->PlaySound_Z(_T("sfx_item_claymoreBrutalPlace-001_soundWave.ogg"), SOUND_UI, 0.3f);
	Sound_Yes();
	return S_OK;
}

HRESULT CInventory::Swap_Slot(CInven_ItemIcon* pItemIcon)
{

	EQUIPSLOTUSAGE Usage = pItemIcon->Get_Usage();

	if (m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage] == nullptr)
	{
		pItemIcon->Resize(m_AInvenValues[INVENTYPE::EQUIP_SLOT][Usage]->Get_Position_Value());
		_int iindex = 0;
		for (int i = 0; i < 12; ++i)
		{
			if (m_AInvenValues[INVENTYPE::ITEM][i] == pItemIcon)
			{
				iindex = i;
				break;
			}
		}
		m_AInvenValues[INVENTYPE::ITEM][iindex] = nullptr;
		m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage] = pItemIcon;
		m_pPlayer->Change_Equipment(pItemIcon->Get_ItemNumber());
		/*Change_Equipment*/
		return S_OK;
	}


	_int index = 0;
	for (int i = 0; i<12; ++i)
	{
		if (m_AInvenValues[INVENTYPE::ITEM][i] == pItemIcon)
		{
			index = i;
			break;
		}
		if (i == 11)
			return E_FAIL;
	}

	_float4 DstPosition = m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage]->Get_Position_Value();
	_float4 SrcPosition = pItemIcon->Get_Position_Value();

	pItemIcon->Resize(DstPosition);
	m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage]->Resize(SrcPosition);

	CInvenUI* SrcPointer = m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage];
	CInven_ItemIcon* DstPointer = pItemIcon;

	m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage] = nullptr;
	m_AInvenValues[INVENTYPE::ITEM][index] = nullptr;
	m_AInvenValues[INVENTYPE::EQUIP_ITEM][Usage] = DstPointer;
	m_AInvenValues[INVENTYPE::ITEM][index] = SrcPointer;
	m_pPlayer->Change_Equipment(pItemIcon->Get_ItemNumber());

	m_pGameInstance->PlaySound_Z(_T("sfx_item_claymoreBrutalPlace-001_soundWave.ogg"), SOUND_UI, 0.3f);
	return S_OK;
}

void CInventory::Clear()
{

}

void CInventory::Render()
{

}

void CInventory::Sound_Yes()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerYes-003_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerYes-001_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerYes-002_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerPurchase-001_soundWave.ogg"), SOUND_UI, 0.3f);
}

void CInventory::Sound_No()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerNo-001_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerNo-002_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_villagerNo-003_soundWave.ogg"), SOUND_UI, 0.3f);
		break;
	}
}



void CInventory::Free()
{

}
