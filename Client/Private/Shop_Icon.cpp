#include "stdafx.h"
#include "Shop_Icon.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inventory.h"
#include "Shop_Info.h"
#include "ShopRarity.h"
#include "ShopPrice.h"
#include "ShopNomoney.h"
#include "Item.h"
#include "Inventory.h"


CShopIcon::CShopIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopIcon::CShopIcon(const CShopIcon& rhs)
	: CShopUI{ rhs }
{
}


HRESULT CShopIcon::Add_Components(const wchar_t* TextureTag)
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopIcon::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	return S_OK;
}

void CShopIcon::Set_Rarity()
{
	switch (m_ItemNum)
	{
	case 1:
		m_eRarity = COMMON;
		m_Price = 50;
		break;
	case 2:
		m_eRarity = UNIQUE;
		break;
	case 3:
		m_eRarity = RARE;
		break;
	case 4:
		m_eRarity = RARE;
		m_Price = 250;
		break;
	case 5:
		m_eRarity = COMMON;
		break;
	case 6:
		m_eRarity = COMMON;
		break;
	case 7:
		m_eRarity = UNIQUE;
		m_Price = 500;
		break;
	case 8:
		m_eRarity = RARE;
		m_Price = 200;
		break;
	case 9:
		m_eRarity = UNIQUE;
		break;
	case 10:
		m_eRarity = COMMON;
		break;
	case 11:
		m_eRarity = COMMON;
		break;
	case 12:
		m_eRarity = COMMON;
		m_Price = 80;
		break;
	default:
		m_eRarity = COMMON;
		m_Price = 0;
		break;
	}
}

void CShopIcon::CreateItem_To_Inventory()
{




}

void CShopIcon::Sound_No()
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




HRESULT CShopIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopIcon::Initialize(void* pArg)
{

	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_OwnDesc = *((ITEMINFO*)pArg);
	if (FAILED(Add_Components(((ITEMINFO*)pArg)->TextureTag)))
		return E_FAIL;

	Set_Unique_ID("Inven_ItemIcon");

	CShop_Info::ITEMINFOINFO infodesc{};

	infodesc.ItemName = ((ITEMINFO*)pArg)->ItemName;
	infodesc.iItemIndex = ((ITEMINFO*)pArg)->ItemIndex;
	infodesc.In_Pos = _float2(1000.f, 450.f);
	infodesc.In_Size = _float2(300.f, 300.f);


	//m_eType
	m_ItemNum = ((ITEMINFO*)pArg)->ItemIndex;
	Set_Rarity();
	if (2 <= m_ItemNum && m_ItemNum <= 7 || m_ItemNum == 10)
	{
		m_eType = EQUIP_WEAPON;
	}
	else if (m_ItemNum == 8 || m_ItemNum == 9)
	{
		m_eType = EQUIP_ARMOR;
	}
	else if (m_ItemNum == 11)
	{
		m_eType = EQUIP_BOW;
	}
	else
	{
		m_eType = EQIP_END;
	}

	infodesc.eRarity = m_eRarity;
	m_ItemInfo = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop_Info"), &infodesc);



	CShopRarity::RARITYDESC rdsc{};
	rdsc.In_Pos = _float2(m_fX, m_fY);
	rdsc.In_Size = _float2(m_fSizeX * 0.9f, m_fSizeY * 0.9f);
	rdsc.eRarity = m_eRarity;
	m_Rarity = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop_Rarity"), &rdsc);


	CShopPrice::PRICEDESC priceDesc{};
	priceDesc.In_Pos = _float2(m_fX - 50.f, m_fY + 50.f);
	priceDesc.In_Size = _float2(30.f, 40.f);
	priceDesc.iPrice = m_Price;
	
	m_pPrice = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop_Price"), &priceDesc);


	return S_OK;
}

void CShopIcon::Priority_Tick(_float fTimeDelta)
{


}


void CShopIcon::Tick(_float fTimeDelta)
{

	if (m_Buyed == true)
		m_pGameInstance->Erase(this);

	if (S_isOpen)
	{
		__super::Tick(fTimeDelta);
		m_Rarity->Tick(fTimeDelta);
		m_pPrice->Tick(fTimeDelta);
		if (isMouseOn)
		{
			if (m_pGameInstance->isKeyDown(VK_RBUTTON))
			{
				if (m_Price <= *CInventory::GetInstance()->Get_NumEmerald())
				{
					if(m_ItemNum == 1 || m_ItemNum == 12)
						CInventory::GetInstance()->Minus_Emerald(m_Price);
					else
					{
						CInventory::GetInstance()->Minus_Emerald(m_Price);
						m_pGameInstance->Erase(this);
					}
					CInventory::GetInstance()->Buy_Shop_Item(this);
					CShopNomoney::WARNINGDESC warningdesc{};
					warningdesc.bType = true;
					CGameObject* NoM = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop_Nomoney"), &warningdesc);
					m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Warning"), NoM);
				}
				else
				{
					CShopNomoney::WARNINGDESC warningdesc{};
					warningdesc.bType = false;
					CGameObject* NoM = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop_Nomoney"),&warningdesc);
					m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Warning"), NoM);
					Sound_No();
				}
			}
			m_ItemInfo->Tick(fTimeDelta);
		}
		
	}
	else
		return;
}


void CShopIcon::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
	{
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FOURTH);
		m_Rarity->Late_Tick(fTimeDelta);
		m_pPrice->Late_Tick(fTimeDelta);
		if (isMouseOn)
			m_ItemInfo->Late_Tick(fTimeDelta);
	}
}

HRESULT CShopIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	/*if (isMouseOn)
	{
		m_pGameInstance->Render_Font(TEXT("Font_Default"), m_ItemName, _float2(800.f, 150.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	}*/

	return S_OK;
}

CShopIcon* CShopIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopIcon* pInstance = new CShopIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopIcon::Clone(void* pArg)
{
	CShopIcon* pInstance = new CShopIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopIcon::Free()
{
	__super::Free();
	Safe_Release(m_ItemInfo);
	Safe_Release(m_Rarity);
	Safe_Release(m_pPrice);
}
