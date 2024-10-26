#include "stdafx.h"
#include "EpicPillar.h"
#include "GameInstance.h"
#include "UIManager.h"

CEpicPillar::CEpicPillar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CEpicPillar::CEpicPillar(const CEpicPillar& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CEpicPillar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Pillar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEpicPillar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_fFrame)))
		return E_FAIL;

	return S_OK;
}


HRESULT CEpicPillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEpicPillar::Initialize(void* pArg)
{

	parentMat = ((EpicPillarDesc*)pArg)->WorldTrans;
	m_IsDead = ((EpicPillarDesc*)pArg)->Isdead;
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;




	Set_Unique_ID("EpicBeam2");
	return S_OK;
}

void CEpicPillar::Priority_Tick(_float fTimeDelta)
{


}


void CEpicPillar::Tick(_float fTimeDelta)
{
	vPos = { parentMat->_41, parentMat->_42 + 3.f,parentMat->_43,1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));


	m_fFrame += fTimeDelta * 25.f;
	if (m_fFrame > m_MaxFrame)
	{
		m_fFrame = 0.f;
	}

	if(*m_IsDead == true || m_IsDead == nullptr)
		m_pGameInstance->Erase(this);
	
}

void CEpicPillar::Late_Tick(_float fTimeDelta)
{
	//if (!S_isOpen)
	//{
	m_pTransformCom->BillBoard();
	m_pTransformCom->Set_Scale(1.f, 6.f, 0.f);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	//CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SIXTH);
	//}
}

HRESULT CEpicPillar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(7);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CEpicPillar* CEpicPillar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEpicPillar* pInstance = new CEpicPillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEpicPillar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEpicPillar::Clone(void* pArg)
{
	CEpicPillar* pInstance = new CEpicPillar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEpicPillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CEpicPillar::Free()
{
	__super::Free();

}
