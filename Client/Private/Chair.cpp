#include "stdafx.h"
#include "Chair.h"

#include "GameInstance.h"

CChair::CChair(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment{ pDevice, pContext }
{
}

CChair::CChair(const CChair & rhs)
	: CEnvironment{ rhs }
{
}

HRESULT CChair::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChair::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Chair");


	return S_OK;
}

void CChair::Priority_Tick(_float fTimeDelta)
{
}

void CChair::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);



}

void CChair::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CChair::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	return S_OK;
}



HRESULT CChair::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chair"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	



	return S_OK;
}

HRESULT CChair::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", m_bIsPicked)))
		return E_FAIL;

	return S_OK;
}





CChair * CChair::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChair*		pInstance = new CChair(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CChair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChair::Clone(void * pArg)
{
	CChair*		pInstance = new CChair(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CChair");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChair::Free()
{
	__super::Free();

}
