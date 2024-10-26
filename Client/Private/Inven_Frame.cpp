#include "stdafx.h"
#include "Inven_Frame.h"
#include "GameInstance.h"
#include "UIManager.h"

CInven_Frame::CInven_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CInven_Frame::CInven_Frame(const CInven_Frame& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CInven_Frame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInven_Frame::Bind_ShaderResources()
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




HRESULT CInven_Frame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInven_Frame::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.8f, 1.f));


	//m_fX = m_fX - g_iWinSizeX * 0.5f;
	//m_fY = -m_fY + g_iWinSizeY * 0.8f;

	Set_Unique_ID("Inven_Frame");
	return S_OK;
}

void CInven_Frame::Priority_Tick(_float fTimeDelta)
{


}


void CInven_Frame::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
		__super::Tick(fTimeDelta);
	else
		return;
}


void CInven_Frame::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
	
}

HRESULT CInven_Frame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CInven_Frame* CInven_Frame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInven_Frame* pInstance = new CInven_Frame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CInven_Frame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInven_Frame::Clone(void* pArg)
{

	CInven_Frame* pInstance = new CInven_Frame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInven_Frame");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CInven_Frame::Free()
{
	__super::Free();

}
