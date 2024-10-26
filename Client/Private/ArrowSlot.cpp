#include "stdafx.h"
#include "ArrowSlot.h"
#include "GameInstance.h"
#include "UIManager.h"

CArrowSlot::CArrowSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CArrowSlot::CArrowSlot(const CArrowSlot& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CArrowSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ArrowSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CArrowSlot::Bind_ShaderResources()
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




HRESULT CArrowSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArrowSlot::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	Set_Unique_ID("Hotbarslot");
	return S_OK;
}

void CArrowSlot::Priority_Tick(_float fTimeDelta)
{


}


void CArrowSlot::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CArrowSlot::Late_Tick(_float fTimeDelta)
{
	if(!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CArrowSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CArrowSlot* CArrowSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrowSlot* pInstance = new CArrowSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CArrowSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArrowSlot::Clone(void* pArg)
{
	CArrowSlot* pInstance = new CArrowSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CArrowSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CArrowSlot::Free()
{
	__super::Free();

}
