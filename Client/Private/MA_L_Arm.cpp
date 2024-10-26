#include "stdafx.h"
#include "MA_L_Arm.h"

#include "GameInstance.h"

CMA_L_Arm::CMA_L_Arm(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CArmor{ pDevice, pContext }
{
}

CMA_L_Arm::CMA_L_Arm(const CMA_L_Arm & rhs)
	: CArmor{ rhs }
{
}

HRESULT CMA_L_Arm::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMA_L_Arm::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(1.f, 1.f, 1.f);
	m_pTransformCom->Rotation_Z(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Rotation_Z(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.35f, 0.f, 1.f));
	return S_OK;
}

void CMA_L_Arm::Priority_Tick(_float fTimeDelta)
{

}

void CMA_L_Arm::Tick(_float fTimeDelta)
{
	
}

void CMA_L_Arm::Late_Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));




	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMA_L_Arm::Render()
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

HRESULT CMA_L_Arm::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_L_Arm"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	


	return S_OK;
}

HRESULT CMA_L_Arm::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CMA_L_Arm * CMA_L_Arm::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMA_L_Arm*		pInstance = new CMA_L_Arm(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMA_L_Arm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMA_L_Arm::Clone(void * pArg)
{
	CMA_L_Arm*		pInstance = new CMA_L_Arm(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMA_L_Arm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMA_L_Arm::Free()
{
	__super::Free();

}
