#include "stdafx.h"
#include "RedStoneBoss.h"
#include "AttackBox.h"
#include "GameInstance.h"
#include "Player.h"
#include "BossHpBar.h"
#include "FreeCamera.h"
#include "ParticleManager.h"
#include "Core.h"
#include "RSS.h"
#include "Door.h"

CRedStoneBoss::CRedStoneBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CRedStoneBoss::CRedStoneBoss(const CRedStoneBoss & rhs)
	: CMonster{ rhs }
{
}

HRESULT CRedStoneBoss::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CRedStoneBoss::Initialize(void * pArg)
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

	Set_Unique_ID("RedStoneBoss");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, false));
	m_pModelCom->StopAnimation(true);
	m_ComBehavior->Set_Activate(false);


	//CBOSSBARDESC* bar;
	


	//m_HpBar = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BossHPBar"), &bardesc);
	//if (m_HpBar == nullptr)
	//	return E_FAIL;

	return S_OK;
}

void CRedStoneBoss::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);

	/*m_HpBar->Priority_Tick(fTimeDelta);*/
}

void CRedStoneBoss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);
	if (IsPlayerApproach(m_DetectDist) && !m_bIsDetected)
	{
		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->StopSound(SOUND_BGM2);
		m_pGameInstance->StopSound(SOUND_BGM3);
		m_pGameInstance->PlayBGM(_T("bgm_mob_fifoBossLoop-001_soundWave.OGG"), 0.3f);
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityAwaken-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
		m_bIsDetected = true;
		m_pModelCom->StopAnimation(false);
		CBossHpBar::CBOSSBARDESC bardesc = {};
		bardesc.MaxHp = &m_pStatus->fMaxHp;
		bardesc.CurrentHp = &m_pStatus->fHp;
		bardesc.FontString = TEXT("RedStone Monstrosity");
		bardesc.FontPos = _float2(g_iWinSizeX / 2 - 310.f, 50.f);
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossHPBar"), &bardesc);
		
		m_Camera->Change_Target(this);
	}

	if (m_pModelCom->Get_AnimFinished() && m_bIsDetected && m_pCurState == STATE_AWAKE)
	{
		m_Camera->Set_Target(m_pPlayer);
		m_ComBehavior->Set_Activate(true);
	}
		



	m_ComBehavior->Update(fTimeDelta);
	
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	/*m_HpBar->Tick(fTimeDelta);*/
}

void CRedStoneBoss::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

	m_pNavigationCom->Compute_Height(m_pTransformCom);


	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);

	/*m_HpBar->Late_Tick(fTimeDelta);*/
}

HRESULT CRedStoneBoss::Render()
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

HRESULT CRedStoneBoss::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 900.f;
	Stat->fMaxHp = 900.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CRedStoneBoss::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneBoss"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(1.6f, 1.6f, 1.6f);
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

HRESULT CRedStoneBoss::AddPartsObjects()
{
	CAttackBox::ATTACKBOX_DESC		CollisionDesc{};
	CollisionDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Hand");
	CollisionDesc.eType = CCollider::TYPE::TYPE_SPHERE;
	CollisionDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollisionDesc.vRadius = 1.5f;

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

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_Head");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);


	m_Camera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));
	return S_OK;
}

HRESULT CRedStoneBoss::Bind_ShaderResources()
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

HRESULT CRedStoneBoss::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("TopSelector"), CBehaviorTree::Selector);

	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("HitSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("HitSelector"), TEXT("Defeat"), CBehaviorTree::Action, bind(&CRedStoneBoss::Defeat, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("HitSelector"), TEXT("Stun"), CBehaviorTree::Action, bind(&CRedStoneBoss::Stun, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("AttackSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("SplitAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::SplitAttack, this, std::placeholders::_1), CoolDown, 15.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("LavaAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::LavaAttack, this, std::placeholders::_1), CoolDown, 20.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("SlamAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::SlamAttack, this, std::placeholders::_1), CoolDown, 10.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("StrongAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::StrongAttack, this, std::placeholders::_1), CoolDown, 4.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("BasicAttack"), CBehaviorTree::Action, bind(&CRedStoneBoss::BasicAttack, this, std::placeholders::_1), CoolDown, 3.f);
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CRedStoneBoss::MoveToPlayer, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("AttackSelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CRedStoneBoss::IDLE, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CRedStoneBoss::Patroll(_float fTimeDelta)
{
	//if (m_bIdle == false)
	//{
	//	if (m_pCurState != STATE_WALK)
	//	{
	//		Turning(fTimeDelta);
	//		return RUNNING;
	//	}
	//	else
	//	{
	//		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	//		return RUNNING;
	//	}
	//}
	//else
	//	return FAILURE;
	return RUNNING;
}

void CRedStoneBoss::Generate_RSS()
{
	RSS::RSSDESC desc{};
	XMStoreFloat4(&desc.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_float4 pos = {};
	XMStoreFloat4(&pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	pos.y += 2.f;
	desc.vPos = pos;
	desc.ParticleNum = 0;
	desc.vColor = _float4(1.f, 0.5f, 0.f, 1.f);
	CGameObject* Rss = nullptr;
	int Random = RandomInt(8, 11);
	for (int i = 0; i < Random; ++i)
	{
		Rss = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RSS"), &desc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), Rss);
	}
	//CParticleManager::GetInstance()->Create_Particle(19, m_PartObjects[2]->Get_ATB_Pos(), nullptr);
	
}

void CRedStoneBoss::Generate_StrongAttack()
{
	ExplosionSound();
	_float4x4 mat1 = *m_PartObjects[0]->Get_ATB_Mat();
	_float4x4 mat2 = *m_PartObjects[1]->Get_ATB_Mat();

	_vector Pos1 = XMVectorSet(mat1._41, mat1._42, mat1._43, 1.f);
	_vector Pos2 = XMVectorSet(mat2._41, mat2._42, mat2._43, 1.f);

	CParticleManager::GetInstance()->Create_Particle(11, Pos1, nullptr);
	CParticleManager::GetInstance()->Create_Particle(11, Pos2, nullptr);
	CParticleManager::GetInstance()->Create_Particle(14, Pos1, nullptr);
	CParticleManager::GetInstance()->Create_Particle(14, Pos2, nullptr);
	CParticleManager::GetInstance()->Create_Particle(15, Pos1, nullptr);
	CParticleManager::GetInstance()->Create_Particle(15, Pos2, nullptr);
}

void CRedStoneBoss::Generate_Missile()
{
	CCore::CoreDesc cordec{};
	cordec.vColor = _float4(1.f, 0.5f, 0.1f, 1.f);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	_vector vPos = m_PartObjects[0]->Get_ATB_Pos();
	cordec.fSpeed = 4.f;
	XMStoreFloat4(&cordec.vDir, vLook);
	XMStoreFloat4(&cordec.vPos, vPos);
	cordec.vPos.y -= 1.8f;
	
	CParticleManager::GetInstance()->Create_Particle(18, XMLoadFloat4(&cordec.vPos), nullptr);



	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);
	CGameObject* Missile = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Missile);


	XMStoreFloat4(&cordec.vDir, XMVectorLerp(vLook , -vRight,0.2f));
	Missile = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Missile);

	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);
	XMStoreFloat4(&cordec.vDir, XMVectorLerp(vLook, vRight, 0.2f));
	//XMStoreFloat4(&cordec.vDir, XMVector3Normalize(vLook + vRight));
	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);

	Missile = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Core"), &cordec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), Missile);

}



void CRedStoneBoss::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 0, false };
	switch (m_pCurState)
	{
	case STATE_AWAKE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_BASIC_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		break;
	case STATE_LAVA_ATTACK:
		m_Animspeed = 0.7f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_STRONG_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		break;
	case STATE_SLAMATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_CHASE:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 10;
		break;
	case STATE_STUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
		break;
	case STATE_DEFEAT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_SPLIT_ATTACK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		break;

	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta* m_Animspeed);

}

NodeStates CRedStoneBoss::MoveToPlayer(_float fTimeDelta)
{
	if (IsPlayerApproach(2.f))
		return FAILURE;
	else
	{
		if(m_pModelCom->Get_Ratio_Betwin(0.28,0.3f) || m_pModelCom->Get_Ratio_Betwin(0.65, 0.67))
			m_Camera->Set_ShakeCamera(0.2f, 0.2f, false);

		if (m_pModelCom->Get_Ratio_Betwin(0.f, 0.02f) || m_pModelCom->Get_Ratio_Betwin(0.50f, 0.52f))
			m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityStep-001_soundWave.ogg"), SOUND_BOSS, 0.3f);

		TurnToPlayer(fTimeDelta);
		m_pCurState = STATE_CHASE;
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		return RUNNING;
	}
}

NodeStates CRedStoneBoss::SplitAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_SLAMATTACK || m_pCurState == STATE_STRONG_ATTACK || m_pCurState == STATE_BASIC_ATTACK
		|| m_pCurState == STATE_LAVA_ATTACK)
		return COOLING;


	if (IsPlayerApproach(6.f) && m_pCurState != STATE_SPLIT_ATTACK)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityGrowl-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
		
		m_pCurState = STATE_SPLIT_ATTACK;
		return RUNNING;
	}

	static _bool Shooted = false;
	if (m_pCurState == STATE_SPLIT_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			Shooted = false;
			return SUCCESS;
		}
		else
		{
			if(m_pModelCom->Get_Current_Ratio() < 0.4f)
				TurnToPlayer(fTimeDelta);

			if (m_pModelCom->Get_Current_Ratio() > 0.4f && !Shooted)
			{
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShoot-003_soundWave.ogg"), SOUND_BOSS, 0.3f);
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShoot-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
				
				Generate_RSS();
				Shooted = true;
			}
			return RUNNING;
		}
	}
	else
		return FAILURE;
}

NodeStates CRedStoneBoss::LavaAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_SLAMATTACK || m_pCurState == STATE_STRONG_ATTACK || m_pCurState == STATE_BASIC_ATTACK)
		return COOLING;


	if (IsPlayerApproach(6.f) && m_pCurState != STATE_LAVA_ATTACK)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityVoiceCharge-003_soundWave.ogg"), SOUND_BOSS, 0.3f);
		m_pCurState = STATE_LAVA_ATTACK;
		return RUNNING;
	}

	static _bool ShakeCamera = false;
	if (m_pCurState == STATE_LAVA_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			ShakeCamera = false;
			return SUCCESS;
		}
		else
		{
			static _float fAccTime = 0.4f;
			static _float fAccTime2 = 0.3f;
			
			if (m_pModelCom->Get_Ratio_Betwin(0.26f, 0.27f) && !ShakeCamera)
			{
				m_Camera->Set_ShakeCamera(0.2f, 3.f, false);
				m_pGameInstance->PlaySound_Z(_T("EarthQuake.mp3"), SOUND_EFFECT, 0.5f);
				ShakeCamera = true;

			}
			if (m_pModelCom->Get_Ratio_Betwin(0.28f, 0.50f))
			{
				fAccTime -= fTimeDelta;
				if (fAccTime < 0.f)
				{
					MakeCircle(3.f, 2.5f, 1, m_pPlayer);
					fAccTime = 0.4f;
				}

				fAccTime2 -= fTimeDelta;
				if (fAccTime2 < 0.f)
				{
					RandomCircle(3.f, 2.5f, 1, 6.f);
					fAccTime2 = 0.3f;
				}

			}



			return RUNNING;
		}
	}
	else
		return FAILURE;

}

NodeStates CRedStoneBoss::SlamAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_STRONG_ATTACK || m_pCurState == STATE_BASIC_ATTACK)
		return COOLING;

	if (IsPlayerApproach(3.f) && m_pCurState != STATE_SLAMATTACK)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShortGrowl-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
		MakeCircle(10.f, 3.f,0);
		m_pCurState = STATE_SLAMATTACK;
		return RUNNING;
	}

	if (m_pCurState == STATE_SLAMATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Current_Ratio() > 0.51f && m_pModelCom->Get_Current_Ratio() < 0.52f)
			{
				m_Camera->Set_ShakeCamera(0.5f, 0.5f, false);
				ExplosionSound();
			}
			return RUNNING;
		}
	}
	else
		return FAILURE;

}

NodeStates CRedStoneBoss::StrongAttack(_float fTimeDelta)
{
	if (m_pCurState == STATE_BASIC_ATTACK)
		return COOLING;

	if (IsPlayerApproach(2.f) && m_pCurState != STATE_STRONG_ATTACK)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityIdle-002_soundWave.ogg"), SOUND_BOSS, 0.3f);
		m_pCurState = STATE_STRONG_ATTACK;
		return RUNNING;
	}
	static _bool Strong= false;
	if (m_pCurState == STATE_STRONG_ATTACK)
	{
		
		if (m_pModelCom->Get_AnimFinished())
		{
			Strong = false;
			return SUCCESS;
		}
		else
		{
			if(m_pModelCom->Get_Current_Ratio() < 0.37f)
				TurnToPlayer(fTimeDelta);
			
			
			if (m_pModelCom->Get_Current_Ratio() > 0.37f && !Strong)
			{
				m_Camera->Set_ShakeCamera(0.5f, 0.5f, false);
				Generate_StrongAttack();
				CCollider* playerCol = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
				for (auto& iter : m_PartObjects)
				{
					if (iter->InterSect(playerCol))
					{
						static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
						static_cast<CPlayer*>(m_pPlayer)->Set_Damage(30.f);
						break;
					}
						
				}
				Strong = true;
				
			}
			return RUNNING;
		}

	}
	else
		return FAILURE;
}

NodeStates CRedStoneBoss::BasicAttack(_float fTimeDelta)
{


	static _bool bActivated = false;
	if (!IsPlayerApproach(2.f) && m_pCurState != STATE_BASIC_ATTACK)
	{
		
		m_pCurState = STATE_BASIC_ATTACK;
		return RUNNING;
	}

	if (m_pCurState == STATE_BASIC_ATTACK)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			bActivated = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Current_Ratio() < 0.48f)
				TurnToPlayer(fTimeDelta);
			
			if (m_pModelCom->Get_Current_Ratio() > 0.48f && !bActivated)
			{
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShoot-003_soundWave.ogg"), SOUND_BOSS, 0.3f);
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShoot-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
				m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityShoot-002_soundWave.ogg"), SOUND_BOSS, 0.3f);
				Generate_Missile();
				bActivated = true;
			}
			
			return RUNNING;
		}
	}
	else
		return FAILURE;

}



NodeStates CRedStoneBoss::IDLE(_float fTimeDelta)
{
	TurnToPlayer(fTimeDelta);
	m_pCurState = STATE_IDLE;
	return RUNNING;
}

NodeStates CRedStoneBoss::Hit(_float fTimeDelta)
{
	return FAILURE;
}

NodeStates CRedStoneBoss::Defeat(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f && m_pCurState != STATE_DEFEAT)
	{
		m_pCurState = STATE_DEFEAT;
		m_pGameInstance->StopAll();
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityDeath-001_soundWave.ogg"), SOUND_BOSS, 0.3f);
		m_pGameInstance->PlaySound_Z(_T("sfx_item_xpWidgetLevelup-001_soundWave.ogg"), SOUND_EFFECT, 0.3f);
		CParticleManager::GetInstance()->Create_Particle(69, static_cast<CPlayer*>(m_pPlayer)->Get_Pos(), nullptr);
		return RUNNING;
	}
	if (m_pCurState == STATE_DEFEAT)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			//m_ComBehavior->Set_Activate(false);
			m_pModelCom->StopAnimation(true);
			g_Threshold += fTimeDelta * 0.3f;
			if (g_Threshold > 1.f)
			{
				g_Threshold = 1.f;
				//m_Camera->SetAngle(182.201324f);
				_vector vTargetPos = XMVectorLerp(XMVectorSet(-9.77, 18.51, -116.90, 1.f), XMVectorSet(-9.80, 18.51, -112.16, 1.f), 0.5f);
				m_Camera->Set_ChangeTarget_Point(vTargetPos);
				m_Camera->Set_ShakeCamera(0.1f, 5.f, true);
				m_pGameInstance->PlaySound_Z(_T("EarthQuake.mp3"), SOUND_EFFECT, 0.5f);
				m_pGameInstance->PlaySound_Z(_T("DoorOpen.ogg"), SOUND_EFFECT, 0.3f);
				//m_Camera->Change_Target(static_cast<CDoor*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Door"), 1)));
				static_cast<CDoor*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Door"), 1))->Set_Door_Open();
				m_pGameInstance->Erase(this);
			}
			return RUNNING;
		}
	}
	else
		return FAILURE;
}

NodeStates CRedStoneBoss::Stun(_float fTimeDelta)
{
	if (m_pCurState != STATE_STUN)
		return COOLING;
	else
	{
		if (m_pModelCom->Get_AnimFinished())
			return SUCCESS;
		else
			return RUNNING;
	}
}

void CRedStoneBoss::HurtSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityHurt-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityHurt-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityHurt-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_rsMonstrosityHurt-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}

}

void CRedStoneBoss::ExplosionSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}


}

CRedStoneBoss * CRedStoneBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRedStoneBoss*		pInstance = new CRedStoneBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRedStoneBoss::Clone(void * pArg)
{
	CRedStoneBoss*		pInstance = new CRedStoneBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRedStoneBoss::Free()
{
	__super::Free();

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	m_PartObjects.clear();
}
