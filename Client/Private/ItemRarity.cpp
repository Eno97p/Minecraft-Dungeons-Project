#include "stdafx.h"
#include "ItemRarity.h"
#include "GameInstance.h"
#include "UIManager.h"

CItemRarity::CItemRarity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CItemRarity::CItemRarity(const CItemRarity& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CItemRarity::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rarity"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemRarity::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", RarityIndex)))
		return E_FAIL;



	return S_OK;
}





HRESULT CItemRarity::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemRarity::Initialize(void* pArg)
{

	RarityIndex = ((RARITYDESC*)pArg)->eRarity;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Inven_ItemInfo");
	return S_OK;
}

void CItemRarity::Priority_Tick(_float fTimeDelta)
{


}


void CItemRarity::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CItemRarity::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
}

HRESULT CItemRarity::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	//m_ItemInfo
	return S_OK;
}

CItemRarity* CItemRarity::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CItemRarity* pInstance = new CItemRarity(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItemRarity");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemRarity::Clone(void* pArg)
{
	CItemRarity* pInstance = new CItemRarity(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItemRarity");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CItemRarity::Free()
{
	__super::Free();

}
