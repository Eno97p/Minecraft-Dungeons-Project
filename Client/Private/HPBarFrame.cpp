#include "stdafx.h"
#include "HPBarFrame.h"
#include "GameInstance.h"
#include "UIManager.h"

CHPBarFrame::CHPBarFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CHPBarFrame::CHPBarFrame(const CHPBarFrame& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CHPBarFrame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Frame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHPBarFrame::Bind_ShaderResources()
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




HRESULT CHPBarFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHPBarFrame::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;



	Set_Unique_ID("HP_Frame");
	return S_OK;
}

void CHPBarFrame::Priority_Tick(_float fTimeDelta)
{


}


void CHPBarFrame::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		__super::Tick(fTimeDelta);
	else
		return;
}


void CHPBarFrame::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CHPBarFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CHPBarFrame* CHPBarFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHPBarFrame* pInstance = new CHPBarFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHPBarFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHPBarFrame::Clone(void* pArg)
{

	CHPBarFrame* pInstance = new CHPBarFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHPBarFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHPBarFrame::Free()
{
	__super::Free();

}
