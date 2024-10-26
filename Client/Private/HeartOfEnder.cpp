#include "stdafx.h"
#include "HeartOfEnder.h"
#include "GameInstance.h"
#include "Player.h"
#include "BossHpBar.h"
#include "FreeCamera.h"
#include "Core.h"
#include "Core_Bomb.h"
#include "RSS.h"
#include "ParticleManager.h"
#include "Lazer.h"
#include "AttackBox.h"

CHeartOfEnder::CHeartOfEnder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CHeartOfEnder::CHeartOfEnder(const CHeartOfEnder & rhs)
	: CMonster{ rhs }
{
}

HRESULT CHeartOfEnder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeartOfEnder::Initialize(void * pArg)
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

	Set_Unique_ID("HeartOfEnder");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(6, false));
	m_pModelCom->StopAnimation(true);
	m_ComBehavior->Set_Activate(false);



	return S_OK;
}

void CHeartOfEnder::Priority_Tick(_float fTimeDelta)
{
	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);
}

void CHeartOfEnder::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);
	if (!m_bIsDetected)
	{
		m_bIsDetected = true;
		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->PlayBGM(_T("bgm_mob_obpiBossStage1Loop-001.OGG"), 0.35f);
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselTeleIn.ogg"), SOUND_BOSS, 0.3f);
		CBossHpBar::CBOSSBARDESC bardesc = {};
		bardesc.MaxHp = &m_pStatus->fMaxHp;
		bardesc.CurrentHp = &m_pStatus->fHp;
		bardesc.FontString = TEXT("Heart Of Ender");
		bardesc.FontPos = _float2(g_iWinSizeX / 2 - 200.f, 50.f);
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_BossHPBar"), &bardesc);
		m_Camera->Change_Target(this);
		m_pModelCom->StopAnimation(false);
		m_pCurState = STATE_APPEAR;
	}

	if (m_pModelCom->Get_AnimFinished() && m_bIsDetected && m_pCurState == STATE_APPEAR)
	{
		m_Camera->Set_Target(m_pPlayer);
		m_ComBehavior->Set_Activate(true);
	}

	if(m_bIsDetected && !m_IsHide)
		CParticleManager::GetInstance()->Create_Particle(52, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	
	m_ComBehavior->Update(fTimeDelta);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);
}

void CHeartOfEnder::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
	m_pNavigationCom->Compute_Height(m_pTransformCom);

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);
}

HRESULT CHeartOfEnder::Render()
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

HRESULT CHeartOfEnder::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 2500.f;
	Stat->fMaxHp = 2500.f;
	Stat->m_fRotationpersec = XMConvertToRadians(90.f);
	Stat->m_fSpeed = 7.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CHeartOfEnder::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HeartOfEnder"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.6f, 1.5f, 0.6f);
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

HRESULT CHeartOfEnder::AddPartsObjects()
{
	m_Camera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));


	CAttackBox::ATTACKBOX_DESC		CollisionDesc{};
	CollisionDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("RootNode");
	CollisionDesc.eType = CCollider::TYPE::TYPE_SPHERE;
	CollisionDesc.vCenter = _float3(0.f, 0.f, 0.f);
	CollisionDesc.vRadius = 0.3f;

	CAttackBox* m_AttackBox = nullptr;
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_BodyAvatar");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Upper_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Upper_FrontArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Middle_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Lower_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Upper_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Upper_FrontArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Middle_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Middle_ForeArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Lower_BackArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Lower_ForeArm");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_NeckAvatar");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	CollisionDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_InnerMouth");
	m_AttackBox = static_cast<CAttackBox*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AttackBox"), &CollisionDesc));
	if (nullptr == m_AttackBox)
		return E_FAIL;

	m_PartObjects.push_back(m_AttackBox);

	PartObjSize = m_PartObjects.size();

	return S_OK;
}

HRESULT CHeartOfEnder::Bind_ShaderResources()
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

HRESULT CHeartOfEnder::Add_Nodes()
{

	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("TopSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("Defeat"), CBehaviorTree::Action, bind(&CHeartOfEnder::Defeat, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("PatternSelector"), CBehaviorTree::Selector, nullptr, CoolDown, 3.f);
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("RandomPattern"), CBehaviorTree::Action, bind(&CHeartOfEnder::RandomPattern, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("Intro"), CBehaviorTree::Action, bind(&CHeartOfEnder::FourSideIntro, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("FourSide"), CBehaviorTree::Action, bind(&CHeartOfEnder::FourSide, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("Split"), CBehaviorTree::Action, bind(&CHeartOfEnder::Split, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("PatternSelector"), TEXT("Summon"), CBehaviorTree::Action, bind(&CHeartOfEnder::Summon, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("Move"), CBehaviorTree::Action , bind(&CHeartOfEnder::SpiderWalk, this, std::placeholders::_1) , CoolDown , 4.f);
	m_ComBehavior->Add_Node(TEXT("TopSelector"), TEXT("Idle"), CBehaviorTree::Action, bind(&CHeartOfEnder::IDLE, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CHeartOfEnder::Patroll(_float fTimeDelta)
{
	return RUNNING;
}



void CHeartOfEnder::SetAnimIndex(_float fTimeDelta)
{

	CModel::ANIMATION_DESC		AnimDesc{ 2, false };
	switch (m_pCurState)
	{
	case STATE_DEFEAT:
		m_Animspeed = 0.5f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_SUMMON_VISAGE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		break;
	case STATE_FOURSIDE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 1;
		break;
	case STATE_INTRO:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_WALK:
		m_Animspeed = 4.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_SPLIT:
		m_Animspeed = 0.8f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_APPEAR:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		break;

	}
	//m_Animspeed = 0.2f;

	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * m_Animspeed);

}

void CHeartOfEnder::Generate_RSS()
{
	SpitVoice();
	RSS::RSSDESC desc{};
	XMStoreFloat4(&desc.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	_float4 pos = {};
	XMStoreFloat4(&pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	pos.y += 2.f;
	desc.vPos = pos;
	desc.ParticleNum = 1;
	desc.vColor = _float4(0.5f, 0.f, 1.f, 1.f);
	CGameObject* Rss = nullptr;
	int Random = RandomInt(3, 5);
	for (int i = 0; i < Random; ++i)
	{
	/*	_int Sign = RandomSign();
		if (Sign > 0)
			desc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
		else
			*/
		Rss = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RSS"), &desc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), Rss);
	}
}

void CHeartOfEnder::Generate_Fourside_Lazer()
{
	VoiceSound();
	CLazer::LAZERDESC desc{};
	desc.fRotationPerSec = 10.f;
	desc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	desc.vZSize = 6.f;
	desc.LifeTime = 4.f;
	//desc.eDirType = (CLazer::LAZERDIRECTION::RIGHT_LOOK);
	CGameObject* Lazer = nullptr;
	for (int i = 1; i < 5; ++i)
	{
		desc.eDirType = (CLazer::LAZERDIRECTION)i;
		desc.fStartRotation = 0;
		for (int j = 0; j < 4; ++j)
		{
			Lazer = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Lazer"), &desc);
			m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), Lazer);
			desc.fStartRotation += 45.f;
		}
	}


}

NodeStates CHeartOfEnder::Defeat(_float fTimeDelta)
{
	//static _float DefeatX = 0.05f;
	if (m_pStatus->fHp <= 0.f && m_pCurState != STATE_DEFEAT)
	{
		m_pGameInstance->StopSound(SOUND_BGM);
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_archVesselOrbDeath-003.ogg"), SOUND_BOSS, 0.3f);
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselDeath.ogg"), SOUND_BOSS, 0.3f);
		m_pCurState = STATE_DEFEAT;
		return RUNNING;
	}
	else if (m_pCurState == STATE_DEFEAT)
	{

		g_Threshold += fTimeDelta * 0.15f;
		if (g_Threshold > 1.f)
		{
			g_Threshold = 1.f;
			m_pGameInstance->Erase(this);
		}

		if(m_pModelCom->Get_Current_Ratio() < 0.3f)
			DeafeatExplosion();
		return RUNNING;
		
			
	}
	else
		return FAILURE;

}

NodeStates CHeartOfEnder::RandomPattern(_float fTimeDelta)
{
	if (m_pCurState >= 1 && m_pCurState <= 5)
		return FAILURE;
	else
	{
		m_pCurState = (HEARTOFENDERSTATES)RandomInt(1, 3);
		return COOLING;
	}
}

NodeStates CHeartOfEnder::Summon(_float fTimeDelta)
{
	if (m_pCurState != STATE_SUMMON_VISAGE)
		return FAILURE;
	else
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_IDLE;
			for (auto& iter : TeleSound)
				iter = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Ratio_Betwin(0.01f, 0.1f))
			{
				if (!TeleSound[0])
				{
					m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselTeleOut.ogg"), SOUND_BOSS, 0.3f);
					TeleSound[0] = true;
				}
				vScale -= fTimeDelta *2.f;
				if (vScale < 0.f)
					vScale = 0.f;
				m_pTransformCom->Set_Scale(vScale, vScale, vScale);
			}

			if (m_pModelCom->Get_Ratio_Betwin(0.5f, 0.8f))
			{
				if (!TeleSound[1])
				{
					m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselTeleIn.ogg"), SOUND_BOSS, 0.3f);
					TeleSound[1] = true;
				}

				vScale += fTimeDelta;
				if (vScale > 1.f)
					vScale = 1.f;
				m_pTransformCom->Set_Scale(vScale, vScale, vScale);
			}

			if (m_pModelCom->Get_Ratio_Betwin(0.1f, 0.8f))
				m_IsHide = true;
			else
				m_IsHide = false;


			//_float a = m_pModelCom->Get_Current_Ratio();
			//cout << a << endl;

			if (m_pModelCom->Get_Current_Ratio() > 0.2f)
				static_cast<CPlayer*>(m_pPlayer)->Summon_Monster(fTimeDelta);


			static _bool isTeleported = false;
			if (m_pModelCom->Get_Ratio_Betwin(0.4f, 0.42f) && !isTeleported)
			{
				Teleport();
				isTeleported = true;
			}

			if (m_pModelCom->Get_Current_Ratio() > 0.5f)
				isTeleported = false;

			return RUNNING;
		}

	}
}

NodeStates CHeartOfEnder::FourSideIntro(_float fTimeDelta)
{
	if (m_pCurState != STATE_INTRO)
		return FAILURE;
	else
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_FOURSIDE;
			Generate_Fourside_Lazer();
			FourSideSound();
			return COOLING;
		}
		else
			return RUNNING;
	}
}

NodeStates CHeartOfEnder::FourSide(_float fTimeDelta)
{
	if (m_pCurState != STATE_FOURSIDE)
		return FAILURE;
	else
	{
		if (m_pModelCom->Get_Current_Ratio() < 0.3f)
			BeamLoop(fTimeDelta);


		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta, 50.f);
		static _float SideTime = 0.f;
		SideTime += fTimeDelta;
		if(SideTime > 5.f)
		{
			SideTime = 0.f;
			m_pCurState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			CParticleManager::GetInstance()->Create_Particle(38, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			return RUNNING;  //레이저 쏘는 로직 추가
		}
	}
}

NodeStates CHeartOfEnder::Split(_float fTimeDelta)
{
	if (m_pCurState != STATE_SPLIT)
		return FAILURE;
	else
	{
		TurnToPlayer(fTimeDelta);
		if (m_pModelCom->Get_AnimFinished())
		{
			m_pCurState = STATE_IDLE;
			for (auto& iter : Splited)
				iter = false;
			return SUCCESS;
		}
		else
		{
			if (m_pModelCom->Get_Current_Ratio() > 0.5f && !Splited[3])
			{
				Generate_RSS();
				Splited[3] = true;
			}
			else if (m_pModelCom->Get_Current_Ratio() > 0.35f && !Splited[2])
			{
				Generate_RSS();
				Splited[2] = true;
			}
			else if (m_pModelCom->Get_Current_Ratio() > 0.25f && !Splited[1])
			{
				Generate_RSS();
				Splited[1] = true;
			}
			else if (m_pModelCom->Get_Current_Ratio() > 0.15f && !Splited[0])
			{
				Generate_RSS();
				Splited[0] = true;
			}


			return RUNNING;
		}
	}
}

NodeStates CHeartOfEnder::SpiderWalk(_float fTimeDelta)
{
	if (m_pCurState != STATE_WALK)
	{
		m_pCurState = STATE_WALK;
		return RUNNING;
	}
	else
	{
		static _float WalkTimeAcc = 0.f;
		WalkTimeAcc += fTimeDelta;
		if (WalkTimeAcc > 0.7f)
		{
			WalkTimeAcc = 0.f;
			Rotation_Random_Four();
			WalkCount++;
		}
		if (WalkCount < 2)
			StepLoop(fTimeDelta);
		
		if (WalkCount > 3)
		{
			WalkCount = 0;
			m_pCurState = STATE_IDLE;
			return SUCCESS;
		}
		
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}
}

NodeStates CHeartOfEnder::IDLE(_float fTimeDelta)
{
	TurnToPlayer(fTimeDelta);
	
	if (m_pCurState != STATE_IDLE)
	{
		VoiceSound();
		m_pCurState = STATE_IDLE;
	}
	
	return SUCCESS;
}

void CHeartOfEnder::DeafeatExplosion()
{
	m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-001_soundWave.ogg"), SOUND_EFFECT, 0.1f);
	_int Random = RandomInt(0, PartObjSize - 1);
	CParticleManager::GetInstance()->Create_Particle(66, m_PartObjects[Random]->Get_ATB_Pos(), nullptr);
}

void CHeartOfEnder::Teleport()
{
	m_pNavigationCom->Teleport_Random_Cell_Approach(m_pTransformCom, 4.f);
	
}





void CHeartOfEnder::Create_Core()
{
	CCore::CoreDesc cordec{};
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	XMStoreFloat4(&cordec.vDir, vLook);
	XMStoreFloat4(&cordec.vPos, vPos);
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);


	XMStoreFloat4(&cordec.vDir, XMVector3Normalize(vLook - vRight));

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);

	XMStoreFloat4(&cordec.vDir, XMVector3Normalize(vLook + vRight));

	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);

}

void CHeartOfEnder::Rotation_Random_Four()
{
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f * RandomSign()));
}




void CHeartOfEnder::HurtSound()
{
	_int RandomSound = RandomInt(0, 7);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-001.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-002.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-003.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-004.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-005.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-006.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 6:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-007.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 7:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselHurt-008.ogg"), SOUND_BOSS, 0.3f);
		break;

	}

}

void CHeartOfEnder::VoiceSound()
{
	_int RandomSound = RandomInt(0, 9);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-001.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-002.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-003.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-004.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-005.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-006.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 6:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-007.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 7:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-008.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 8:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-009.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 9:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselArchieVoice-010.ogg"), SOUND_BOSS, 0.3f);
		break;

	}

}

void CHeartOfEnder::FourSideSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselFourside-004.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselFourside-003.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselFourside-001.ogg"), SOUND_BOSS, 0.3f);
		break;
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselFourside-002.ogg"), SOUND_BOSS, 0.3f);
}

void CHeartOfEnder::SpitVoice()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselVesselVoice-001.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselVesselVoice-002.ogg"), SOUND_BOSS, 0.3f);
		break;
	}
}

void CHeartOfEnder::StepVoice()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselStep-001.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselStep-002.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselStep-003.ogg"), SOUND_BOSS, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvesselStep-004.ogg"), SOUND_BOSS, 0.3f);
		break;
	}

}


void CHeartOfEnder::BeamLoop(_float fTimeDelta)
{
	fBeamInterval -= fTimeDelta;
	if (fBeamInterval < 0.f)
	{
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchvisageBeamLoop-001.ogg"), SOUND_BOSS, 0.3f);
		fBeamInterval = 1.f;
	}
}

void CHeartOfEnder::StepLoop(_float fTimeDelta)
{
	fStepInterval -= fTimeDelta;
	if (fStepInterval < 0.f)
	{
		StepVoice();
		fStepInterval = 2.f;
	}
}

CHeartOfEnder * CHeartOfEnder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHeartOfEnder*		pInstance = new CHeartOfEnder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHeartOfEnder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHeartOfEnder::Clone(void * pArg)
{
	CHeartOfEnder*		pInstance = new CHeartOfEnder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHeartOfEnder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHeartOfEnder::Free()
{
	__super::Free();
	for (auto& iter : m_PartObjects)
		Safe_Release(iter);

}
