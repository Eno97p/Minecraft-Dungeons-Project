#include "stdafx.h"
#include "Inven_MainSlot.h"
#include "GameInstance.h"
#include "UIManager.h"

CInven_MainSlot::CInven_MainSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CInven_MainSlot::CInven_MainSlot(const CInven_MainSlot& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CInven_MainSlot::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenMainSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInven_MainSlot::Bind_ShaderResources()
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




HRESULT CInven_MainSlot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInven_MainSlot::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	//m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, m_fY, 0.f, 1.f));

	
	


	Set_Unique_ID("Inven_MainSlot");
	return S_OK;
}

void CInven_MainSlot::Priority_Tick(_float fTimeDelta)
{


}


void CInven_MainSlot::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CInven_MainSlot::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CInven_MainSlot::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CInven_MainSlot* CInven_MainSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInven_MainSlot* pInstance = new CInven_MainSlot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CInven_MainSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInven_MainSlot::Clone(void* pArg)
{
	CInven_MainSlot* pInstance = new CInven_MainSlot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInven_MainSlot");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CInven_MainSlot::Free()
{
	__super::Free();

}
