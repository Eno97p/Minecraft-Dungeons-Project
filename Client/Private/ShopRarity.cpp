#include "stdafx.h"
#include "ShopRarity.h"
#include "GameInstance.h"
#include "UIManager.h"

CShopRarity::CShopRarity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopRarity::CShopRarity(const CShopRarity& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CShopRarity::Add_Components()
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

HRESULT CShopRarity::Bind_ShaderResources()
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





HRESULT CShopRarity::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopRarity::Initialize(void* pArg)
{

	RarityIndex = ((RARITYDESC*)pArg)->eRarity;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Inven_ItemInfo");
	return S_OK;
}

void CShopRarity::Priority_Tick(_float fTimeDelta)
{


}


void CShopRarity::Tick(_float fTimeDelta)
{
	if (S_isOpen)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}

void CShopRarity::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
}

HRESULT CShopRarity::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	//m_ItemInfo
	return S_OK;
}

CShopRarity* CShopRarity::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopRarity* pInstance = new CShopRarity(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopRarity");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopRarity::Clone(void* pArg)
{
	CShopRarity* pInstance = new CShopRarity(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopRarity");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopRarity::Free()
{
	__super::Free();

}
