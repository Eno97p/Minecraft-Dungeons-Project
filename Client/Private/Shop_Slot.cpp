#include "stdafx.h"
#include "Shop_Slot.h"
#include "GameInstance.h"
#include "UIManager.h"

CShopSlot::CShopSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopSlot::CShopSlot(const CShopSlot& rhs)
	: CShopUI{ rhs }
	, m_bSlotPicked{rhs.m_bSlotPicked }
{
}

HRESULT CShopSlot::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_UI"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopSlot::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_IsMouseOn", &isMouseOn, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;


	return S_OK;
}




HRESULT CShopSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopSlot::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	

	Set_Unique_ID("Shop_Slot");
	return S_OK;
}

void CShopSlot::Priority_Tick(_float fTimeDelta)
{


}


void CShopSlot::Tick(_float fTimeDelta)
{
	if (S_isOpen)
	{
		if (m_pGameInstance->isKeyDown(VK_LBUTTON))
		{
			if (isMouseOn)
				m_bSlotPicked = true;
			else
				m_bSlotPicked = false;
		}
		
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CShopSlot::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CShopSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CShopSlot* CShopSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopSlot* pInstance = new CShopSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopSlot::Clone(void* pArg)
{
	CShopSlot* pInstance = new CShopSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopSlot::Free()
{
	__super::Free();

}
