#include "stdafx.h"
#include "NPC.h"
#include "FKey.h"
#include "GameInstance.h"

CNPC::CNPC(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment{ pDevice, pContext }
{
}

CNPC::CNPC(const CNPC & rhs)
	: CEnvironment{ rhs }
{
}

HRESULT CNPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("NPC");

	CModel::ANIMATION_DESC desc = {0,true};

	m_pModelCom->Set_AnimationIndex(desc);
	CFKey::FONTKEYDESC fontdesc{};


	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = XMVector3Normalize( m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * -1.f;
	_vector KeyPos = vPos + vLook * 2.f;
	KeyPos += XMVectorSet(0.f, 1.f, 0.f, 0.f) * 0.5f;
	XMStoreFloat4(&fontdesc.vPos, KeyPos);
	
	m_fKey = static_cast<CFKey*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_FKey"), &fontdesc));

	return S_OK;
}

void CNPC::Priority_Tick(_float fTimeDelta)
{
}

void CNPC::Tick(_float fTimeDelta)
{

	m_pModelCom->Play_Animation(fTimeDelta);


}

void CNPC::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		if (IsPlayerApproach(5.f))
		{
			m_fKey->Tick(fTimeDelta);
			m_fKey->Late_Tick(fTimeDelta);
		}
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CNPC::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;



	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}


	return S_OK;
}

HRESULT CNPC::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	return S_OK;
}

HRESULT CNPC::Bind_ShaderResources()
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

CNPC * CNPC::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNPC*		pInstance = new CNPC(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CNpc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CNPC::Clone(void * pArg)
{
	CNPC*		pInstance = new CNPC(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CNpc");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC::Free()
{
	Safe_Release(m_fKey);
	__super::Free();
	
}
