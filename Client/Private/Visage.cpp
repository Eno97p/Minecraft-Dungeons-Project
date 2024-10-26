#include "stdafx.h"
#include "Visage.h"

#include "GameInstance.h"
#include "Lazer.h"

CVisage::CVisage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster{ pDevice, pContext }
{
}

CVisage::CVisage(const CVisage & rhs)
	: CMonster{ rhs }
	,m_Out{rhs.m_Out }
{
}

HRESULT CVisage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVisage::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Visage");

	CModel::ANIMATION_DESC desc = {0,false};

	m_pModelCom->Set_AnimationIndex(desc);
	
	m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageTeleIn.ogg"), SOUND_MONSTER, 0.3f);

	return S_OK;
}

void CVisage::Priority_Tick(_float fTimeDelta)
{
}

void CVisage::Tick(_float fTimeDelta)
{

	m_pModelCom->Play_Animation(fTimeDelta);
	//if (m_pModelCom->Get_AnimFinished())
	//	m_pGameInstance->Erase(this);
	if(m_pModelCom->Get_Current_Ratio() > 0.99f)
		m_pGameInstance->Erase(this);


	if (!m_Out && m_pModelCom->Get_Current_Ratio() > 0.7f)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageTeleOut.ogg"), SOUND_MONSTER, 0.3f);
		m_Out = true;
	}

	if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.5f))
		BeamLoop(fTimeDelta);




	if (m_pModelCom->Get_Current_Ratio() < 0.2f)
	{
		TurnToPlayer(fTimeDelta);
	}
	else
	{
		if (!m_Bool)
		{
			m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageTeleOut.ogg"), SOUND_MONSTER, 0.3f);
			BeamAttackSound();
			Generate_Lazer();
			m_Bool = true;
		}
	}
	


}

void CVisage::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	}
}

HRESULT CVisage::Render()
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

HRESULT CVisage::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Visage"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	


	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.3f, 0.5f, 0.3f);
	aabbdesc.vCenter = _float3(0.f, aabbdesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbdesc)))
		return E_FAIL;


	if (FAILED(AddNavigation()))
		return E_FAIL;


	return S_OK;
}

HRESULT CVisage::Bind_ShaderResources()
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

HRESULT CVisage::Add_Nodes()
{
	return S_OK;
}

NodeStates CVisage::Patroll(_float fTimeDelta)
{
	return NodeStates();
}

void CVisage::Generate_Lazer()
{
	CLazer::LAZERDESC desc{};
	desc.fRotationPerSec = 10.f;
	desc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	desc.vZSize = 6.f;
	desc.LifeTime = 3.f;
	//Prototype_GameObject_Lazer
	CGameObject* Lazer = nullptr;
	desc.fStartRotation = 0;
	for (int i = 0; i < 4; i++)
	{
		Lazer = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Lazer"), &desc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), Lazer);
		desc.fStartRotation += 45.f;
	}
}

void CVisage::BeamAttackSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageBeamAttack-001.ogg"), SOUND_MONSTER, 0.2f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageBeamAttack-002.ogg"), SOUND_MONSTER, 0.2f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageBeamAttack-003.ogg"), SOUND_MONSTER, 0.2f);
		break;
	}

}

void CVisage::BeamLoop(_float fTimeDelta)
{
	fBeamInterval -= fTimeDelta;
	if (fBeamInterval < 0.f)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageBeamLoop-001.ogg"), SOUND_MONSTER, 0.3f);
		fBeamInterval = 1.f;
	}

}

CVisage * CVisage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVisage*		pInstance = new CVisage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CVisage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVisage::Clone(void * pArg)
{
	CVisage*		pInstance = new CVisage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CVisage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVisage::Free()
{
	__super::Free();
}

HRESULT CVisage::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 500.f;
	Stat->fMaxHp = 500.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}
