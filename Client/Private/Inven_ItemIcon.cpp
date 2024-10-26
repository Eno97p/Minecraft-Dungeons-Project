#include "stdafx.h"
#include "Inven_ItemIcon.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inven_Item_Info.h"
#include "Inventory.h"
#include "ItemRarity.h"

CInven_ItemIcon::CInven_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CInven_ItemIcon::CInven_ItemIcon(const CInven_ItemIcon& rhs)
	: CInvenUI{ rhs }
{
}

void CInven_ItemIcon::Resize(_float4 PositionValue)
{
	CUI::Resize(PositionValue);

	static_cast<CUI*>(m_Rarity)->Resize(_float4(m_fX, m_fY, m_fSizeX * 0.9f, m_fSizeY * 0.9f));

}

HRESULT CInven_ItemIcon::Add_Components(const wchar_t* TextureTag)
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

HRESULT CInven_ItemIcon::Bind_ShaderResources()
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

void CInven_ItemIcon::Set_Rarity()
{
	switch (m_ItemNum)
	{
	case 2:
		m_eRarity = UNIQUE;
		break;
	case 3:
		m_eRarity = RARE;
		break;
	case 4:
		m_eRarity = RARE;
		break;
	case 5:
		m_eRarity = COMMON;
		break;
	case 6:
		m_eRarity = COMMON;
		break;
	case 7:
		m_eRarity = UNIQUE;
		break;
	case 8:
		m_eRarity = RARE;
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
	default:
		m_eRarity = COMMON;
		break;
	}
}




HRESULT CInven_ItemIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInven_ItemIcon::Initialize(void* pArg)
{

	

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(((ITEMINFO*)pArg)->TextureTag)))
		return E_FAIL;

	Set_Unique_ID("Shop_ItemIcon");

	CInven_ItemINFO::ITEMINFOINFO infodesc{};

	infodesc.ItemName = ((ITEMINFO*)pArg)->ItemName;
	infodesc.TextureTag = ((ITEMINFO*)pArg)->TextureTag;
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
	m_ItemInfo = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Inven_Item_Info"), &infodesc);

	CItemRarity::RARITYDESC rdsc{};
	rdsc.In_Pos = _float2(m_fX, m_fY);
	rdsc.In_Size = _float2(m_fSizeX * 0.9f, m_fSizeY *0.9f);
	rdsc.eRarity = m_eRarity;
	m_Rarity = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item_Rarity"), &rdsc);


	return S_OK;
}

void CInven_ItemIcon::Priority_Tick(_float fTimeDelta)
{


}


void CInven_ItemIcon::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
		m_Rarity->Tick(fTimeDelta);
		if (isMouseOn)
		{
			if (m_pGameInstance->isKeyDown(VK_RBUTTON))
			{
				CInventory::GetInstance()->Swap_Slot(this);
			}
			m_ItemInfo->Tick(fTimeDelta);
		}
	}
	else
		return;
}


void CInven_ItemIcon::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FOURTH);
		m_Rarity->Late_Tick(fTimeDelta);
		if (isMouseOn)
			m_ItemInfo->Late_Tick(fTimeDelta);
	}
}

HRESULT CInven_ItemIcon::Render()
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

CInven_ItemIcon* CInven_ItemIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInven_ItemIcon* pInstance = new CInven_ItemIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CInven_ItemIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInven_ItemIcon::Clone(void* pArg)
{
	CInven_ItemIcon* pInstance = new CInven_ItemIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInven_ItemIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CInven_ItemIcon::Free()
{
	__super::Free();
	Safe_Release(m_ItemInfo);
	Safe_Release(m_Rarity);
}
