#include "stdafx.h"
#include "HotBarSlot.h"
#include "GameInstance.h"
#include "UIManager.h"

CHotBarSlot::CHotBarSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CHotBarSlot::CHotBarSlot(const CHotBarSlot& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CHotBarSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hot_Bar_Slot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHotBarSlot::Bind_ShaderResources()
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




HRESULT CHotBarSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHotBarSlot::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	Set_Unique_ID("Hotbarslot");
	return S_OK;
}

void CHotBarSlot::Priority_Tick(_float fTimeDelta)
{


}


void CHotBarSlot::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CHotBarSlot::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CHotBarSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CHotBarSlot* CHotBarSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHotBarSlot* pInstance = new CHotBarSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHotBarSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHotBarSlot::Clone(void* pArg)
{
	CHotBarSlot* pInstance = new CHotBarSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHotBarSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHotBarSlot::Free()
{
	__super::Free();

}
