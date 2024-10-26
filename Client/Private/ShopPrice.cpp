#include "stdafx.h"
#include "ShopPrice.h"
#include "GameInstance.h"
#include "UIManager.h"

CShopPrice::CShopPrice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopPrice::CShopPrice(const CShopPrice& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CShopPrice::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Emerald"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopPrice::Bind_ShaderResources()
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





HRESULT CShopPrice::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopPrice::Initialize(void* pArg)
{

	m_iPrice = ((PRICEDESC*)pArg)->iPrice;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("ShopPrice");
	return S_OK;
}

void CShopPrice::Priority_Tick(_float fTimeDelta)
{


}


void CShopPrice::Tick(_float fTimeDelta)
{
	if (S_isOpen)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}

void CShopPrice::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIFTH);
}

HRESULT CShopPrice::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	m_pGameInstance->Render_Font(TEXT("Font_Middle"), to_wstring(m_iPrice), _float2(m_fX + 15.f, m_fY - 20.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	//m_ItemInfo
	return S_OK;
}

CShopPrice* CShopPrice::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopPrice* pInstance = new CShopPrice(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopPrice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopPrice::Clone(void* pArg)
{
	CShopPrice* pInstance = new CShopPrice(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopPrice");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopPrice::Free()
{
	__super::Free();

}
