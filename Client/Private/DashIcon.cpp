#include "stdafx.h"
#include "DashIcon.h"
#include "GameInstance.h"
#include "UIManager.h"

CDashIcon::CDashIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CDashIcon::CDashIcon(const CDashIcon& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CDashIcon::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DashIcon"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDashIcon::Bind_ShaderResources()
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




HRESULT CDashIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDashIcon::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	Set_Unique_ID("HotBar_Frame");
	return S_OK;
}

void CDashIcon::Priority_Tick(_float fTimeDelta)
{


}


void CDashIcon::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		__super::Tick(fTimeDelta);
	else
		return;
}


void CDashIcon::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
	
}

HRESULT CDashIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	m_pGameInstance->Render_Font(TEXT("Font_TinyEng"), TEXT("Space"), _float2(m_fX - 30.f, m_fY + 15.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

CDashIcon* CDashIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDashIcon* pInstance = new CDashIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDashIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDashIcon::Clone(void* pArg)
{

	CDashIcon* pInstance = new CDashIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDashIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CDashIcon::Free()
{
	__super::Free();

}
