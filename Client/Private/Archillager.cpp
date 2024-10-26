#include "stdafx.h"
#include "Archillager.h"
#include "GameInstance.h"
#include "Player.h"
#include "BossHpBar.h"
#include "FreeCamera.h"
#include "Core.h"
#include "Core_Bomb.h"
#include "AttackBox.h"
#include "ParticleManager.h"

CArchillager::CArchillager(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CArchillager::CArchillager(const CArchillager & rhs)
	: CMonster{ rhs }
{
}

HRESULT CArchillager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArchillager::Initialize(void * pArg)
{

	m_DetectDist = 30.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(AddPartsObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Set_Unique_ID("Archillager");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(7, false));
	m_pModelCom->StopAnimation(true);
	m_ComBehavior->Set_Activate(false);

	return S_OK;
}

void CArchillager::Priority_Tick(_float fTimeDelta)
{
	m_Orb->Priority_Tick(fTimeDelta);
}

void CArchillager::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);
	if (IsPlayerApproach(m_DetectDist) && !m_bIsDetected)
	{
		m_pGameInstance->StopAll();
		m_pGameInstance->PlayBGM(_T("bgm_env_pupa2DArena-001_soundWave.OGG"), 0.3f);
		m_bIsDetected = true;
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerIntroThirdFly.ogg"), SOUND_MONSTER, 0.3f);
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerIntroLaugh-002.ogg"), SOUND_MONSTER, 0.3f);
		CBossHpBar::CBOSSBARDESC bardesc = {};
		bardesc.MaxHp = &m_pStatus->fMaxHp;
		bardesc.CurrentHp = &m_pStatus->fHp;
		bardesc.FontString = TEXT("Arch - Illager");
		bardesc.FontPos = _float2(g_iWinSizeX / 2 - 200.f, 50.f);
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossHPBar"), &bardesc);
		m_Camera->Change_Target(this);
		m_pModelCom->StopAnimation(false);
		
	}

	if (m_pCurState == STATE_APPEAR)
	{
		if(m_pModelCom->Get_Ratio_Betwin(0.01f, 0.2f))
			CParticleManager::GetInstance()->Create_Particle(46, m_Orb->Get_ATB_Pos(), nullptr);

		static _bool Hargh = false;
		if (m_pModelCom->Get_Current_Ratio() > 0.4f && !Hargh)
		{
			m_pGameInstance->PlaySound_Z(_T("sfx_mob_archillagerHargh-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			Hargh = true;
		}

		if (m_pModelCom->Get_Ratio_Betwin(0.5f, 0.8f))
		{
			static _bool ObStruc = false;
			if (!ObStruc)
			{
				m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchIllagerOrbWobbleStruggle.ogg"), SOUND_MONSTER, 0.3f);
				ObStruc = true;
			}
			static _bool ObStrucFin = false;
			if (m_pModelCom->Get_Current_Ratio() > 0.7f && !ObStrucFin)
			{
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_archIllager_spellDissapear-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
				ObStrucFin = true;
			}
			static _bool ObStrucFin2 = false;
			if (m_pModelCom->Get_Current_Ratio() > 0.75f && !ObStrucFin2)
			{
				m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerIntroFirstFly.ogg"), SOUND_MONSTER, 0.3f);
				ObStrucFin2 = true;
			}

			CParticleManager::GetInstance()->Create_Particle(48, m_Orb->Get_ATB_Pos(), nullptr);
		}

		if (m_pModelCom->Get_Ratio_Betwin(0.2f, 0.58f))
			m_Camera->Distance_Up(fTimeDelta);
	}



	if (m_pModelCom->Get_AnimFinished() && m_bIsDetected && m_pCurState == STATE_APPEAR)
	{

		m_Camera->Set_Target(m_pPlayer);
		m_Camera->Reset_Distance();
		m_ComBehavior->Set_Activate(true);
	}




	m_ComBehavior->Update(fTimeDelta);
	m_Orb->Tick(fTimeDelta);

	m_fParticleInterval -= fTimeDelta;
	if(m_bIsDetected && m_fParticleInterval < 0.f)
	{ 
		CParticleManager::GetInstance()->Create_Particle(46, m_Orb->Get_ATB_Pos(), nullptr);
		m_fParticleInterval = 0.1f;
	}
	
	
}

void CArchillager::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	m_pNavigationCom->Compute_Height(m_pTransformCom);
	m_Orb->Late_Tick(fTimeDelta);
}

HRESULT CArchillager::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(2);

		m_pModelCom->Render(i);
	}
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CArchillager::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 2000.f;
	Stat->fMaxHp = 2000.f;
	Stat->m_fRotationpersec = XMConvertToRadians(90.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CArchillager::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Archillager"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.4f, 0.4f, 0.4f);
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

HRESULT CArchillager::AddPartsObjects()
{
	m_Camera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));

	CAttackBox::ATTACKBOX_DESC		CollisionDesc{};
	CollisionDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("_Cube");
	CollisionDesc.eType = CCollider::TYPE::TYPE_SPHERE;
	CollisionDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollisionDesc.vRadius = 0.1f;


	m_Orb = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_Orb)
		return E_FAIL;




	return S_OK;
}

HRESULT CArchillager::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CArchillager::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("TopSelector"), CBehaviorTree::Selector);

	//m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("SlamAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::SlamAttack, this, std::placeholders::_1), CoolDown, 10.f);
	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("Defeat"), CBehaviorTree::Action, bind(&CArchillager::Defeat, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("PatternSelector"), CBehaviorTree::Selector,nullptr, CoolDown, 5.f);

	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("RandomNumber"), CBehaviorTree::Action, bind(&CArchillager::RandomPattern, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("SummonLine"), CBehaviorTree::Action, bind(&CArchillager::Summon_Line, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("SummonCircle"), CBehaviorTree::Action, bind(&CArchillager::Summon_Circle, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("SummonSingle"), CBehaviorTree::Action, bind(&CArchillager::Summon_Single, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("SubSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("SubSelector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CArchillager::Basic_Attack, this, std::placeholders::_1), CoolDown,3.f);
	m_ComBehavior->Add_Node(TEXT("SubSelector"), TEXT("Teleport"), CBehaviorTree::Action, bind(&CArchillager::TeleportToRandom, this, std::placeholders::_1), CoolDown, 3.f);
	m_ComBehavior->Add_Node(TEXT("SubSelector"), TEXT("Land"), CBehaviorTree::Action, bind(&CArchillager::Landing, this, std::placeholders::_1));

	/*m_ComBehavior->Add_Node(TEXT("SubSelector"), TEXT("Move"), CBehaviorTree::Action, bind(&CArchillager::Move, this, std::placeholders::_1));*/
	m_ComBehavior->Add_Node(TEXT("SubSelector"), TEXT("Idle"), CBehaviorTree::Action, bind(&CArchillager::IDLE, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CArchillager::Patroll(_float fTimeDelta)
{
	return RUNNING;
}



void CArchillager::SetAnimIndex(_float fTimeDelta)
{

	CModel::ANIMATION_DESC		AnimDesc{ 7, false };
	switch (m_pCurState)
	{
	case STATE_APPEAR:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		break;
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 9;
		break;
	case STATE_BASIC_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_BASIC_JUMP_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_SUMMON_CIRCLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 14;
		break;
	case STATE_SUMMON_LINE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 15;
		break;
	case STATE_SUMMON_SINGLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 16;
		break;
	case STATE_FLYING:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 8;
		break;
	case STATE_LANDING:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 11;
		break;
	case STATE_RUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 12;
		break;
	case STATE_DEFEAT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		break;

	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * m_Animspeed);

}

void CArchillager::Teleport()
{
	CParticleManager::GetInstance()->Create_Particle(49, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	m_pNavigationCom->Teleport_Random_Cell_Approach(m_pTransformCom, 5.f);
	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_EFFECT, 0.3f);
	RandomSound();
}

void CArchillager::Summon_Monster(_int Loop)
{
	CMonster::MONSTER_DEC dec{};
	CGameObject* monster = nullptr;
	for (int i = 0; i < Loop; ++i)
	{
		dec.Pos = m_pNavigationCom->Get_Random_Cell_Point(6.f);
		CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);
		dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
		dec.Scale = 1.f;

		_int iRandomNumber = RandomInt(0, 1);
		switch (iRandomNumber)
		{
		case 0:
			monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Zombie"), &dec);
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
			break;
		case 1:
			monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Vindicator"), &dec);
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
			break;
		}

	}
	

}

void CArchillager::Summon_HeartOfEnder()
{
	CMonster::MONSTER_DEC dec{};
	CGameObject* monster = nullptr;
	XMStoreFloat4(&dec.Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	dec.RotationAngle = XMConvertToRadians(180.f);
	dec.Scale = 1.f;

	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_HeartOfEnder"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);

}

void CArchillager::Create_Core()
{
	CCore::CoreDesc cordec{};
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
	_vector vLeft = vRight * -1;
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	cordec.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	cordec.ParticleNum = 1;

	XMStoreFloat4(&cordec.vPos, vPos);
	CGameObject* Core = nullptr;

	XMStoreFloat4(&cordec.vDir, vLook);
	Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Core);


	_vector rightdir = XMVectorLerp(vLook, vRight, 0.1f);
	_vector leftdir = XMVectorLerp(vLook, vLeft, 0.1f);

	
	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);
	
	XMStoreFloat4(&cordec.vDir, XMVector3Normalize(rightdir));
	
	Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Core);
	
	XMStoreFloat4(&cordec.vDir, XMVector3Normalize(leftdir));
	
	Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Core);

	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);

}

void CArchillager::Create_Core_Circle()
{
	CCore::CoreDesc cordec{};
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&cordec.vPos, vPos);
	cordec.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	cordec.ParticleNum = 1;
	CGameObject* Core = nullptr;
	float angleIncrement = 360.0f / 8.0f; // 45도 간격
	for (int i = 0; i < 8; ++i) {
		float angle = i * angleIncrement;
		float radian = XMConvertToRadians(angle);
		float x = cosf(radian);
		float z = sinf(radian);

		cordec.vDir = _float4(x, 0.0f, z, 0.f);

		
		Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Core);

		//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
}

void CArchillager::Create_Following_Core()
{
	CGameObject* Core = nullptr;
	CCore_Bomb::CoreBombDesc dec{};
	dec.pTarget = m_pPlayer;
	dec.vPos = m_pNavigationCom->Get_Random_Cell_Point(4.f);
	//XMStoreFloat4(&dec.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	Core = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core_Bomb"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Core);

	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
	//m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerSummonMobs-001.ogg"), SOUND_MONSTER, 0.3f);
	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core_Bomb"), &dec);
}


NodeStates CArchillager::Landing(_float fTimeDelta)
{
	if (m_pCurState == STATE_LANDING)
	{
		TurnToPlayer(fTimeDelta);
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_IDLE;
			return FAILURE;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CArchillager::Defeat(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f && m_pCurState != STATE_DEFEAT)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerWhirlwind.ogg"), SOUND_MONSTER, 0.3f);
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerIntroFinalBlow.ogg"), SOUND_MONSTER, 0.3f);
		CParticleManager::GetInstance()->Create_Particle(53, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		CParticleManager::GetInstance()->Create_Particle(54, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		CParticleManager::GetInstance()->Create_Particle(55, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		m_pCurState = STATE_DEFEAT;
		return RUNNING;
	}
	else if (m_pCurState == STATE_DEFEAT)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			//최종보스 소환
			Summon_HeartOfEnder();
			m_pGameInstance->Erase(this);
		}

		return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CArchillager::Move(_float fTimeDelta)
{
	if (IsPlayerApproach(2.f) && m_pCurState != STATE_RUN)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pCurState = STATE_RUN;
	}
	else if (m_pCurState == STATE_RUN)
	{
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		return RUNNING;
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CArchillager::IDLE(_float fTimeDelta)
{
	TurnToPlayer(fTimeDelta);
	m_pCurState = STATE_IDLE;
	return RUNNING;
}

void CArchillager::RandomSound()
{
	_int RandomSound = RandomInt(0, 9);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerIdle-010.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerIdle-011.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerIdle-012.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerIdle-013.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archIllagerHaha-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archIllagerHaha-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 6:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archIllagerLaugh-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 7:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archillagerHargh-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 8:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archillagerHargh-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 9:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_archillagerHargh-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;


	}

}

void CArchillager::HurtSound()
{
	_int RandomSound = RandomInt(0, 5);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-001.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-002.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-003.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-004.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-005.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerHurt-006.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}

}

NodeStates CArchillager::Basic_Attack(_float fTimeDelta) //쿨 돌리고 
{
	static _bool isCoreSpawn = false;
	if (m_pCurState != STATE_BASIC_ATTACK && m_pCurState != STATE_LANDING)
	{
		m_pCurState = STATE_BASIC_ATTACK;
		if (isCoreSpawn == true)
			isCoreSpawn = false;
		RandomSound();
		m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple - 001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
		return RUNNING;
	}
	else
	{
		TurnToPlayer(fTimeDelta);

		if (m_pModelCom->Get_AnimFinished())
		{
			isCoreSpawn = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Ratio_Betwin(0.48f, 0.50f) && !isCoreSpawn)
			{
				Create_Core();
				isCoreSpawn = true;
			}
			return RUNNING;
		}
	}

}

NodeStates CArchillager::RandomPattern(_float fTimeDelta)
{
	if (m_pCurState == STATE_SUMMON_CIRCLE || m_pCurState == STATE_SUMMON_LINE ||
		m_pCurState == STATE_SUMMON_SINGLE)
		return FAILURE;
	else
	{
		if (m_pCurState == STATE_IDLE || m_pCurState == STATE_RUN)
		{
			m_pCurState = (ARCHILLAGERSTATE)RandomInt(4, 6);
			RandomSound();
			m_pGameInstance->PlaySound_Z(_T("sfx_boss_archillagerSummonMobs-001.ogg"), SOUND_EFFECT, 0.3f);
			return RUNNING;
		}
		else
			return FAILURE;
	}
}

NodeStates CArchillager::Summon_Line(_float fTimeDelta)
{
	if (m_pCurState != STATE_SUMMON_LINE)
	{
		return FAILURE;
	}
	else
	{
		TurnToPlayer(fTimeDelta);
		if (m_pModelCom->Get_AnimFinished())
		{
			Summon_Monster(5);
			m_pCurState = STATE_IDLE;
			return SUCCESS;
		}
		else
			return RUNNING;
	}
}

NodeStates CArchillager::Summon_Circle(_float fTimeDelta)
{

	static _bool IsCreated = false;
	if (m_pCurState != STATE_SUMMON_CIRCLE)
	{
		IsCreated = false;
		return FAILURE;
	}
	else
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_IDLE;
			IsCreated = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Current_Ratio() > 0.51f && !IsCreated)
			{
				Create_Core_Circle();
				IsCreated = true;
			}
			return RUNNING;
		}
			
	}
}

NodeStates CArchillager::Summon_Single(_float fTimeDelta)
{
	if (m_pCurState != STATE_SUMMON_SINGLE)
	{
		return FAILURE;
	}
	else
	{
		TurnToPlayer(fTimeDelta);
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Ratio_Betwin(0.48f, 0.52f))
			{
				Create_Following_Core();
			}
			return RUNNING;
		}

	}
}


NodeStates CArchillager::TeleportToRandom(_float fTimeDelta)	//쿨
{
	if (IsPlayerApproach(3.f) && m_pCurState != STATE_LANDING)
	{
		Teleport();
		m_pCurState = STATE_LANDING;
		CParticleManager::GetInstance()->Create_Particle(51, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		return SUCCESS;
	}
	else
		return FAILURE;
}




CArchillager * CArchillager::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CArchillager*		pInstance = new CArchillager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CArchillager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CArchillager::Clone(void * pArg)
{
	CArchillager*		pInstance = new CArchillager(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CArchillager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CArchillager::Free()
{
	__super::Free();

	Safe_Release(m_Orb);
	/*Safe_Release(m_Camera);*/

}
