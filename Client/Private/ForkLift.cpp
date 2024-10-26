#include "stdafx.h"
#include "ForkLift.h"

#include "GameInstance.h"

CForkLift::CForkLift(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CForkLift::CForkLift(const CForkLift & rhs)
	: CMonster{ rhs }
{
}

HRESULT CForkLift::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CForkLift::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("ForkLift");


	if (FAILED(Add_Nodes()))
		return E_FAIL;

	return S_OK;
}

void CForkLift::Priority_Tick(_float fTimeDelta)
{
}

void CForkLift::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);



}

void CForkLift::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	m_pNavigationCom->Compute_Height(m_pTransformCom);
}

HRESULT CForkLift::Render()
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

#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CForkLift::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 200.f;
	Stat->fMaxHp = 200.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CForkLift::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.6f, 0.6f, 0.6f);
	aabbdesc.vCenter = _float3(0.f, aabbdesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbdesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_ComBehavior))))
	{
		return E_FAIL;
	}

	if (FAILED(AddNavigation()))
		return E_FAIL;


	return S_OK;
}

HRESULT CForkLift::Bind_ShaderResources()
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



HRESULT CForkLift::Add_Nodes()
{


	return S_OK;
}

NodeStates CForkLift::Patroll(_float fTimeDelta)
{
	
	return FAILURE;
}




CForkLift * CForkLift::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CForkLift*		pInstance = new CForkLift(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : ForkLift");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CForkLift::Clone(void * pArg)
{
	CForkLift*		pInstance = new CForkLift(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : ForkLift");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CForkLift::Free()
{
	__super::Free();

}
