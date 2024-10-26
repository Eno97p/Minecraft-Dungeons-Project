#include "stdafx.h"
#include "Golem.h"
#include "AttackBox.h"
#include "GameInstance.h"
#include "Player.h"
#include "Circle.h"
#include "ParticleManager.h"
#include "MonsterHP.h"

_float fVoulume = 0.3f;

CGolem::CGolem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CGolem::CGolem(const CGolem & rhs)
	: CMonster{ rhs }
{
}

HRESULT CGolem::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGolem::Initialize(void * pArg)
{

	m_DetectDist = 15.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(AddPartsObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Set_Unique_ID("Golem");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));

	static_cast<CMonsterHpBar*>(m_SmallHpBar)->Set_YLength(2.5f);
	//m_pTransformCom->Set_Power(2.f);
	return S_OK;
}

void CGolem::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);

	if (m_firsthit)
		m_SmallHpBar->Priority_Tick(fTimeDelta);
}

void CGolem::Tick(_float fTimeDelta)
{

	m_ComBehavior->Update(fTimeDelta);
	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);

	
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	if (m_firsthit)
		m_SmallHpBar->Tick(fTimeDelta);

}

void CGolem::Late_Tick(_float fTimeDelta)
{

	m_pNavigationCom->Compute_Height(m_pTransformCom);


	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);

	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		CollisionEachOther(fTimeDelta);
	}

	if (m_firsthit)
		m_SmallHpBar->Late_Tick(fTimeDelta);
}

HRESULT CGolem::Render()
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
#endif

	return S_OK;
}

HRESULT CGolem::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 400.f;
	Stat->fMaxHp = 400.f;
	Stat->m_fRotationpersec = XMConvertToRadians(90.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CGolem::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneGolem"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(1.2f, 1.2f, 1.2f);
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

HRESULT CGolem::AddPartsObjects()
{
	CAttackBox::ATTACKBOX_DESC		CollisionDesc{};
	CollisionDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Hand");
	CollisionDesc.eType = CCollider::TYPE::TYPE_SPHERE;
	CollisionDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollisionDesc.vRadius = 0.5f;
	CAttackBox* m_AttackBox = nullptr;
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Hand");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	return S_OK;
}

HRESULT CGolem::Bind_ShaderResources()
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

HRESULT CGolem::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Dead"), CBehaviorTree::Action, bind(&CGolem::Dead, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);

	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CGolem::Detect, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("AttackSelector"), CBehaviorTree::Selector);

	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("ChargeAttack"), CBehaviorTree::Action, bind(&CGolem::ChargeAttack, this, std::placeholders::_1), CoolDown, 10.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("StrongAttack"), CBehaviorTree::Action, bind(&CGolem::StrongAttack, this, std::placeholders::_1), CoolDown, 5.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("Swipeattack"), CBehaviorTree::Action, bind(&CGolem::SwipeAttack, this, std::placeholders::_1), CoolDown, 1.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CGolem::MoveToPlayer, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IdleSelector"), CBehaviorTree::Selector);
	//m_ComBehavior->Add_Node(TEXT("IdleSelector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CGolem::Patroll, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("IdleSelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CGolem::IDLE, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CGolem::Patroll(_float fTimeDelta)
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
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}
	else
		return FAILURE;
}



void CGolem::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 2, true };
	switch (m_pCurState)
	{
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_BASIC_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_CHARGE_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		break;
	case STATE_STRONG_ATTACK:
		m_Animspeed = 1.5f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_SUPER_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		break;
	case STATE_SWIPE_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		break;
	case STATE_NOEVENT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_WALK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 8;
		break;
	case STATE_CHASE:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 8;
		break;
	case STATE_STUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_TURNING:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 2;
		break;

	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta* m_Animspeed);

}



NodeStates CGolem::MoveToPlayer(_float fTimeDelta)
{

	if (!IsPlayerApproach(1.f))
	{
		if (m_pModelCom->Get_Ratio_Betwin(0.29f, 0.3f) || m_pModelCom->Get_Ratio_Betwin(0.70f, 0.71f))
			m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemStepShort-001_soundWave.ogg"), SOUND_MONSTER, 0.2f);

		m_pCurState = STATE_CHASE;
		TurnToPlayer(fTimeDelta);
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		return RUNNING;
	}
	else
	{
		m_pCurState = STATE_IDLE;
		return FAILURE;
	}
	
}

NodeStates CGolem::SwipeAttack(_float fTimeDelta)
{
	if (IsPlayerApproach(2.f) && m_pCurState != STATE_SWIPE_ATTACK)
	{
		GrowlSound();
		m_pCurState = STATE_SWIPE_ATTACK;
		return RUNNING;
	}
	else if (!IsPlayerApproach(2.f) && m_pCurState != STATE_SWIPE_ATTACK)
		return FAILURE;

	if (m_pCurState == STATE_SWIPE_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			bSwipeAttack = false;
			return SUCCESS;
		}
		else
		{
			if (!bSwipeAttack)
			{
				if (m_PartObjects[RIGHT_HAND]->InterSect(m_pPlayerCollider))
				{
					m_pGameInstance->PlaySound_Z(_T("sfx_player_splashBreak-002_soundWave"), SOUND_EFFECT, 0.3f);
					CParticleManager::GetInstance()->Create_Particle(7, m_PartObjects[RIGHT_HAND]->Get_ATB_Pos(), nullptr);
					static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
					static_cast<CPlayer*>(m_pPlayer)->Set_Damage(10.f);
					bSwipeAttack = true;
				}
			}
	
			return RUNNING;
		}

	}
	else
	{
		bSwipeAttack = false;
		return FAILURE;
	}
		
}

NodeStates CGolem::ChargeAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_SWIPE_ATTACK || m_pCurState == STATE_STRONG_ATTACK)
		return COOLING;



	if (IsPlayerApproach(5.f) && m_pCurState != STATE_CHARGE_ATTACK)
	{
		GrowlSound();
		m_pGameInstance->PlaySound_Z(_T("ChargeAttack.ogg"), SOUND_MONSTER, 0.3f);
		m_pCurState = STATE_CHARGE_ATTACK;
		return RUNNING;
	}

	if (m_pCurState == STATE_CHARGE_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.7f))
			{
				ChargeTime -= fTimeDelta;
				if (ChargeTime < 0.f)
				{
					RandomCircle(2.f, 2.f, 2, 5.f);
					ChargeTime = 0.2f;
				}
			}
			return RUNNING;
		}
	}
	else
		return FAILURE;
}

NodeStates CGolem::StrongAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_SWIPE_ATTACK)
		return COOLING;

	if (IsPlayerApproach(2.f) && m_pCurState != STATE_STRONG_ATTACK)
	{
		GrowlSound();
		m_pCurState = STATE_STRONG_ATTACK;
		return RUNNING;
	}

	if (m_pCurState == STATE_STRONG_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			StrongAttacked = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Current_Ratio() < 0.3f)
				TurnToPlayer(fTimeDelta);


			if (!StrongAttacked && m_pModelCom->Get_Current_Ratio() > 0.45f)
			{
				StrongAttackSound();

				m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
				_vector One = m_PartObjects[0]->Get_ATB_Pos();
				_vector Two = m_PartObjects[1]->Get_ATB_Pos();
				_vector Final = XMVectorLerp(One, Two, 0.5f);
				CParticleManager::GetInstance()->Create_Particle(63, Final, nullptr);
				CParticleManager::GetInstance()->Create_Particle(64, Final, nullptr);
				if (XMVectorGetX(XMVector3Length(PlayerPos() - Final)) < 1.5f)
				{
					static_cast<CPlayer*>(m_pPlayer)->Set_Damage(15.f);
					static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
				}
				StrongAttacked = true;
			}

			//CCollider* playerCol = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
			//for (auto& iter : m_PartObjects)
			//{
			//	if (iter->InterSect(playerCol))
			//		static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
			//}
			return RUNNING;
		}

	}
	else
	{
		StrongAttacked = false;
		return FAILURE;
	}
}



NodeStates CGolem::Turning(_float fTimeDelta)
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



NodeStates CGolem::IDLE(_float fTimeDelta)
{
	m_pCurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CGolem::Hit(_float fTimeDelta)
{
	return FAILURE;
}

NodeStates CGolem::Stun(_float fTimeDelta)
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

NodeStates CGolem::Dead(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f)
	{
		g_Threshold += fTimeDelta * 0.5f;
		m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta);
		if (g_Threshold > 1.f)
		{
			m_pGameInstance->PlaySound_Z(_T("sfx_redstoneGolemDeath-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			g_Threshold = 1.f;
			NamedDrop();
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
		return FAILURE;
}

void CGolem::SwipeSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstonegolemSwipe-001.ogg"), SOUND_MONSTER, fVoulume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstonegolemSwipe-002.ogg"), SOUND_MONSTER, fVoulume);
		break;
	}

}

void CGolem::StrongAttackSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemSwing-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemSwing-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}

void CGolem::WalkSound()
{
}

void CGolem::DeadSound()
{

}

void CGolem::GrowlSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemGrowl-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemGrowl-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemGrowl-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}

}

void CGolem::HurtSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemHurt-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemHurt-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_redstoneGolemHurt-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}

CGolem * CGolem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGolem*		pInstance = new CGolem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGolem::Clone(void * pArg)
{
	CGolem*		pInstance = new CGolem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolem::Free()
{
	__super::Free();

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
