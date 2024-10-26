#include "stdafx.h"
#include "Vindicator.h"
#include "Player.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "ParticleManager.h"

CVindicator::CVindicator(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CVindicator::CVindicator(const CVindicator & rhs)
	: CMonster{ rhs }
{
}

HRESULT CVindicator::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVindicator::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(AddPartsObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Set_Unique_ID("Vindicator");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));
	m_pTransformCom->Set_Power(2.f);
	return S_OK;
}

void CVindicator::Priority_Tick(_float fTimeDelta)
{
	m_Axe->Priority_Tick(fTimeDelta);

	if (m_firsthit)
		m_SmallHpBar->Priority_Tick(fTimeDelta);
}

void CVindicator::Tick(_float fTimeDelta)
{

	m_ComBehavior->Update(fTimeDelta);
	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);

	m_Axe->Tick(fTimeDelta);

	if (m_firsthit)
		m_SmallHpBar->Tick(fTimeDelta);
}

void CVindicator::Late_Tick(_float fTimeDelta)
{
	if (!IsPlayerApproach(30.f))
		return;

	m_Axe->Late_Tick(fTimeDelta);

	if (m_pTransformCom->isJumping())
	{
		m_pTransformCom->Jump(fTimeDelta, m_pNavigationCom);
	}
	else
		m_pNavigationCom->Compute_Height(m_pTransformCom);

	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		CollisionEachOther(fTimeDelta);
	}
	if (m_firsthit)
		m_SmallHpBar->Late_Tick(fTimeDelta);
}

HRESULT CVindicator::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(5);

		m_pModelCom->Render(i);
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CVindicator::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 70.f;
	Stat->fMaxHp = 70.f;
	Stat->m_fRotationpersec = XMConvertToRadians(120.f);
	Stat->m_fSpeed = 2.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CVindicator::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Vindicator"),
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

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_ComBehavior))))
	{
		return E_FAIL;
	}

	if (FAILED(AddNavigation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVindicator::AddPartsObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_RWeapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	m_Axe = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DoubleAxe"), &WeaponDesc);
	if (nullptr == m_Axe)
		return E_FAIL;

	return S_OK;
}

HRESULT CVindicator::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", m_bIsPicked)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_HitTimeRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Threshold", &g_Threshold, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 1)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVindicator::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Dead"), CBehaviorTree::Action, bind(&CVindicator::Dead, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Hit"), CBehaviorTree::Action, bind(&CVindicator::Hit, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Stun"), CBehaviorTree::Action, bind(&CVindicator::Stun, this, std::placeholders::_1));


	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CVindicator::Detect, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("DetectSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CVindicator::Attack, this, std::placeholders::_1), CoolDown, 3.f);
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CVindicator::MoveToPlayer, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Hold"), CBehaviorTree::Action, bind(&CVindicator::Hold, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLESelector"), CBehaviorTree::Selector);


	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CVindicator::Patroll, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CVindicator::IDLE, this, std::placeholders::_1));
	return S_OK;
}

NodeStates CVindicator::Patroll(_float fTimeDelta)
{
	if (m_bIdle == false)
	{
		if (m_pCurState != STATE_WALK)
		{
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), rand() % 360);
			m_pCurState = STATE_WALK;
			return RUNNING;
		}
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}
	else
		return FAILURE;
}



void CVindicator::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 0, true };
	switch (m_pCurState)
	{
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_RUN:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_WALK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_NOEVENT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		break;
	case STATE_ATTACK:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_STUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		break;
	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta* m_Animspeed);


}

NodeStates CVindicator::MoveToPlayer(_float fTimeDelta)
{
	if (IsPlayerApproach(1.f))
		return FAILURE;
	else
	{
		TurnToPlayer(fTimeDelta);
		m_pCurState = STATE_RUN;
		m_pTransformCom->Go_Straight_Faster(fTimeDelta, m_pNavigationCom);
		return RUNNING;
	}
}



NodeStates CVindicator::Attack(_float fTimeDelta)
{
	if (IsPlayerApproach(1.f) && m_pCurState != STATE_ATTACK)
	{
		m_pCurState = STATE_ATTACK;
		IDLESound();
		return RUNNING;
	}

	if (m_pCurState == STATE_ATTACK)
	{
		
		if (m_pModelCom->Get_AnimFinished())
		{
			IsAttacking = false;
			return SUCCESS;
		}
		else
		{
			if (!IsAttacking)
			{
				if(m_Axe->InterSected(m_pPlayer))
				{
					CParticleManager::GetInstance()->Create_Particle(7, static_cast<CWeapon*>(m_Axe)->Get_Collider_Center(), nullptr);
					static_cast<CPlayer*>(m_pPlayer)->Set_Damage(6.f);
					IsAttacking = true;
				}
			}
			return RUNNING;
		}
	}
	else
	{
		IsAttacking = false;
		return FAILURE;
	}
		

}

NodeStates CVindicator::IDLE(_float fTimeDelta)
{
	m_pCurState = STATE_IDLE;
	return RUNNING;
}

NodeStates CVindicator::Hit(_float fTimeDelta)
{
	return FAILURE;
}

NodeStates CVindicator::Stun(_float fTimeDelta)
{
	if (m_pCurState != STATE_STUN)
		return FAILURE;
	else
	{
		if (m_pModelCom->Get_AnimFinished())
			return FAILURE;
		else
			return RUNNING;
	}
}

NodeStates CVindicator::Hold(_float fTimeDelta)
{
	LookAtPlayer();
	m_pCurState = STATE_IDLE;
	return RUNNING;
}

NodeStates CVindicator::Dead(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f)
	{
		g_Threshold += fTimeDelta;
		m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 1.7f);
		if (g_Threshold > 1.f)
		{
			g_Threshold = 1.f;
			DeadSound();
			RandomDrop();
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
		return FAILURE;
}

void CVindicator::IDLESound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorIdle-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorIdle-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorIdle-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorIdle-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}


}

void CVindicator::HurtSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorHurt-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorHurt-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorHurt-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}

}

void CVindicator::DeadSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorDeath-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_vindicatorDeath-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}


CVindicator * CVindicator::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVindicator*		pInstance = new CVindicator(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Vindicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVindicator::Clone(void * pArg)
{
	CVindicator*		pInstance = new CVindicator(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Vindicator");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVindicator::Free()
{
	__super::Free();
	Safe_Release(m_Axe);

}
