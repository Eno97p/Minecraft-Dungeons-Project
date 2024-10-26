#include "stdafx.h"
#include "Skeleton_Horse.h"
#include "Arrow.h"
#include "GameInstance.h"
#include "Weapon.h"
#include "AttackBox.h"
#include "Player.h"
#include "MonsterHP.h"
#include "ParticleManager.h"
CSkeletonHorse::CSkeletonHorse(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CSkeletonHorse::CSkeletonHorse(const CSkeletonHorse & rhs)
	: CMonster{ rhs }
{
}

HRESULT CSkeletonHorse::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkeletonHorse::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(AddPartsObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;
	Set_Unique_ID("Skeleton_Horse");

	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(1, true));

	static_cast<CMonsterHpBar*>(m_SmallHpBar)->Set_YLength(2.5f);
	return S_OK;
}

void CSkeletonHorse::Priority_Tick(_float fTimeDelta)
{
	if (m_firsthit)
		m_SmallHpBar->Priority_Tick(fTimeDelta);
}

void CSkeletonHorse::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_ComBehavior->Update(fTimeDelta);
	m_Bow->Tick(fTimeDelta);
	m_AttackBox->Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);


	CParticleManager::GetInstance()->Create_Particle(76, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);

	if (m_firsthit)
		m_SmallHpBar->Tick(fTimeDelta);
}

void CSkeletonHorse::Late_Tick(_float fTimeDelta)
{

	if (m_firsthit)
		m_SmallHpBar->Late_Tick(fTimeDelta);

	m_pNavigationCom->Compute_Height(m_pTransformCom);
	m_AttackBox->Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		CollisionEachOther(fTimeDelta);
	}
}

HRESULT CSkeletonHorse::Render()
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
	//m_AttackBox->Render();
#endif

	return S_OK;
}

HRESULT CSkeletonHorse::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 40.f;
	Stat->fHp = 200.f;
	Stat->fMaxHp = 200.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;

}

HRESULT CSkeletonHorse::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton_Horseman"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.3f, 0.8f, 0.3f);
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

HRESULT CSkeletonHorse::AddPartsObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Weapon_ske");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	m_Bow = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Bow"), &WeaponDesc);
	if (nullptr == m_Bow)
		return E_FAIL;


	CAttackBox::ATTACKBOX_DESC		CollisionDesc{};
	CollisionDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("HeadHorse");
	CollisionDesc.eType = CCollider::TYPE::TYPE_SPHERE;
	CollisionDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollisionDesc.vRadius = 0.5f;
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;


	return S_OK;
}

HRESULT CSkeletonHorse::Bind_ShaderResources()
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

HRESULT CSkeletonHorse::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Dead"), CBehaviorTree::Action, bind(&CSkeletonHorse::DEAD, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CSkeletonHorse::Detect, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("DetectSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CSkeletonHorse::Moving, this, std::placeholders::_1));
	/*m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CSkeleton::Attack, this, std::placeholders::_1));*/
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLESelector"), CBehaviorTree::Selector);


	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CSkeletonHorse::Patroll, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CSkeletonHorse::IDLE, this, std::placeholders::_1));
	return S_OK;
}

NodeStates CSkeletonHorse::Patroll(_float fTimeDelta)
{
	if (m_bIdle == false)
	{
		if (m_pCurState != STATE_WALK)
		{
			Turning(fTimeDelta);
			return RUNNING;
		}
		else
		{
			m_pCurState = STATE_WALK;
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}
	else
		return FAILURE;
}

NodeStates CSkeletonHorse::Turning(_float fTimeDelta)
{
	static _float Acctime = 0.f;
	static _int rv = 1;
	if (m_pCurState != STATE_TURNING)
	{
		rv = RandomSign();
		m_pCurState = STATE_TURNING;
		return RUNNING;
	}
	else
	{
		Acctime += fTimeDelta;
		if (Acctime > 3.f)
		{
			Acctime = 0.f;
			m_pCurState = STATE_WALK;
			return FAILURE;
		}
		m_pTransformCom->TurnToRandomValue(fTimeDelta, rv);
		return RUNNING;
	}
}

NodeStates CSkeletonHorse::Moving(_float fTimeDelta)
{
	TurnToPlayer(fTimeDelta);
	m_pTransformCom->Go_Straight_Faster(fTimeDelta, m_pNavigationCom);
	m_pCurState = STATE_RUN;
	

	if (m_pModelCom->Get_Ratio_Betwin(0.25f, 0.3f) || m_pModelCom->Get_Ratio_Betwin(0.6f, 0.65f))
		StepSound();



	if (!IsAttacking)
	{
		if (m_AttackBox->InterSect(m_pPlayerCollider))
		{
			static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
			static_cast<CPlayer*>(m_pPlayer)->Set_Damage(7.f);
			CParticleManager::GetInstance()->Create_Particle(7, PlayerPos(), nullptr);
			IsAttacking = true;
		}
	}
	else
	{
		m_fAttackCooltime -= fTimeDelta;
		if (m_fAttackCooltime < 0.f)
		{
			IsAttacking = false;
			m_fAttackCooltime = 1.f;
		}
	}


	return RUNNING;
}



NodeStates CSkeletonHorse::IDLE(_float fTimeDelta)
{
	m_pCurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CSkeletonHorse::NoEvents(_float fTimeDelta)
{
	return SUCCESS;
}

NodeStates CSkeletonHorse::Spawn(_float fTimeDelta)
{


	return NodeStates();
}

NodeStates CSkeletonHorse::DEAD(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f)
	{
		g_Threshold += fTimeDelta;
		m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta *2.f);
		if (g_Threshold > 1.f)
		{
			g_Threshold = 1.f;
			NamedDrop();
			DeathSound();
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
		return FAILURE;
}

void CSkeletonHorse::StepSound()
{
	_int RandomSound = RandomInt(0, 5);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-005_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseStepSoft-006_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}

}

void CSkeletonHorse::DeathSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseGallop-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseGallop-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseGallop-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseGallop-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
	m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHorseDeath-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
}



void CSkeletonHorse::HurtSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHurt-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHurt-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHurt-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonHurt-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}


void CSkeletonHorse::Shoot_Arrow()
{
	CArrow::ArrowDesc Arrowdec = {};
	Arrowdec.bTypeFrendly = false;

	_vector vDir = Calculate_Player_Dir();
	XMStoreFloat4(&Arrowdec.vDir, vDir);
	XMStoreFloat4(&Arrowdec.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Arrowdec);
}

void CSkeletonHorse::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 2, true };
	switch (m_pCurState)
	{
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 1;
		break;
	case STATE_RUN:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_WALK:
		m_Animspeed = 0.5f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_NOEVENT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_ATTACK:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_TURNING:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 1;
		break;
	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * m_Animspeed);

}

CSkeletonHorse * CSkeletonHorse::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkeletonHorse*		pInstance = new CSkeletonHorse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : SkeletonHorse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkeletonHorse::Clone(void * pArg)
{
	CSkeletonHorse*		pInstance = new CSkeletonHorse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : SkeletonHorse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkeletonHorse::Free()
{
	__super::Free();
	Safe_Release(m_Bow);
	Safe_Release(m_AttackBox);

}
