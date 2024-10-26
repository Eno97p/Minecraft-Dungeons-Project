#include "stdafx.h"
#include "Shop_Frame.h"
#include "GameInstance.h"
#include "UIManager.h"

CShopFrame::CShopFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopFrame::CShopFrame(const CShopFrame& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CShopFrame::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopFrame"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopFrame::Bind_ShaderResources()
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




HRESULT CShopFrame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopFrame::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 900.f;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX - 450.f;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.8f, 1.f));


	//m_fX = m_fX - g_iWinSizeX * 0.5f;
	//m_fY = -m_fY + g_iWinSizeY * 0.8f;

	Set_Unique_ID("Shop_Frame");
	return S_OK;
}

void CShopFrame::Priority_Tick(_float fTimeDelta)
{


}


void CShopFrame::Tick(_float fTimeDelta)
{
	if (S_isOpen)
		__super::Tick(fTimeDelta);
	else
		return;
}


void CShopFrame::Late_Tick(_float fTimeDelta)
{
	if(S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
	
}

HRESULT CShopFrame::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(4);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CShopFrame* CShopFrame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopFrame* pInstance = new CShopFrame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopFrame::Clone(void* pArg)
{

	CShopFrame* pInstance = new CShopFrame(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopFrame");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopFrame::Free()
{
	__super::Free();

}
