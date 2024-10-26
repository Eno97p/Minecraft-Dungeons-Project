#include "stdafx.h"
#include "HotBar_Frame.h"
#include "GameInstance.h"
#include "UIManager.h"

CHotBarFrame::CHotBarFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CHotBarFrame::CHotBarFrame(const CHotBarFrame& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CHotBarFrame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HotBarFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHotBarFrame::Bind_ShaderResources()
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




HRESULT CHotBarFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHotBarFrame::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	Set_Unique_ID("HotBar_Frame");
	return S_OK;
}

void CHotBarFrame::Priority_Tick(_float fTimeDelta)
{


}


void CHotBarFrame::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		__super::Tick(fTimeDelta);
	else
		return;
}


void CHotBarFrame::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
	
}

HRESULT CHotBarFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CHotBarFrame* CHotBarFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHotBarFrame* pInstance = new CHotBarFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHotBarFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHotBarFrame::Clone(void* pArg)
{

	CHotBarFrame* pInstance = new CHotBarFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHotBarFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHotBarFrame::Free()
{
	__super::Free();

}
