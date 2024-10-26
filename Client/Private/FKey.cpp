#include "stdafx.h"
#include "FKey.h"
#include "GameInstance.h"
#include "UIManager.h"

CFKey::CFKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CFKey::CFKey(const CFKey& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CFKey::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FontButton"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CFKey::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}


HRESULT CFKey::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFKey::Initialize(void* pArg)
{

	m_vPos = ((FONTKEYDESC*)pArg)->vPos;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(0.001f, 0.001f, 0.001f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos));

	Set_Unique_ID("FKey");
	return S_OK;
}

void CFKey::Priority_Tick(_float fTimeDelta)
{


}


void CFKey::Tick(_float fTimeDelta)
{



}

void CFKey::Late_Tick(_float fTimeDelta)
{
	if (!S_isOpen)
	{
		m_pTransformCom->BillBoard();
		m_pTransformCom->Set_Scale(0.3f, 0.3f, 0.3f);
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SIXTH);
	}
}

HRESULT CFKey::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(5);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CFKey* CFKey::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFKey* pInstance = new CFKey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFKey");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFKey::Clone(void* pArg)
{
	CFKey* pInstance = new CFKey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFKey");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CFKey::Free()
{
	__super::Free();

}
