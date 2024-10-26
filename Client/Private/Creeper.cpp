#include "stdafx.h"
#include "Creeper.h"
#include "GameInstance.h"
#include "ParticleManager.h"
#include "FreeCamera.h"
#include "Player.h"
#include "MonsterHP.h"

CCreeper::CCreeper(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CCreeper::CCreeper(const CCreeper & rhs)
	: CMonster{ rhs }
{
}

HRESULT CCreeper::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCreeper::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Creeper");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(4, true));

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	m_pTransformCom->Set_Power(3.f);


	return S_OK;
}

void CCreeper::Priority_Tick(_float fTimeDelta)
{
	if (m_firsthit)
		m_SmallHpBar->Priority_Tick(fTimeDelta);
}

void CCreeper::Tick(_float fTimeDelta)
{

	m_ComBehavior->Update(fTimeDelta);
	__super::Tick(fTimeDelta);
	/*m_pModelCom->Play_Animation(fTimeDelta);*/
	SetAnimIndex(fTimeDelta);

	//if(m_pCurState == STATE_WALK)
	//	m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	
	if (m_firsthit)
		m_SmallHpBar->Tick(fTimeDelta);

	
}

void CCreeper::Late_Tick(_float fTimeDelta)
{

	if (m_firsthit)
		m_SmallHpBar->Late_Tick(fTimeDelta);

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

}

HRESULT CCreeper::Render()
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

HRESULT CCreeper::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 40.f;
	Stat->fMaxHp = 40.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CCreeper::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Creeper"),
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

HRESULT CCreeper::Bind_ShaderResources()
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

void CCreeper::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 4, true };

	switch (m_pCurState)
	{
	case STATE_IDLE:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_RUN:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 8;
		break;
	case STATE_WALK:
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 8;
		break;
	case STATE_NOEVENT:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 6;
		break;
	case STATE_STUN:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 7;
		break;
	case STATE_HIT:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		break;
	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta);
}

HRESULT CCreeper::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DEAD"), CBehaviorTree::Action, bind(&CCreeper::Dead, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);

	/*m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CCreeper::Detect, this, std::placeholders::_1));*/

	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Attack"), CBehaviorTree::Action, bind(&CCreeper::Attack, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Hit"), CBehaviorTree::Action, bind(&CCreeper::Hit, this, std::placeholders::_1));
	//m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Move"), CBehaviorTree::Action, bind(&CCreeper::MoveToPlayer, this, std::placeholders::_1));

	//m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLESelector"), CBehaviorTree::Selector);


	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CCreeper::Patroll, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CCreeper::IDLE, this, std::placeholders::_1));

	return S_OK;
}

NodeStates CCreeper::Patroll(_float fTimeDelta)
{
	if (IsAttacking == true)
		return FAILURE;


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


NodeStates CCreeper::MoveToPlayer(_float fTimeDelta)
{
	TurnToPlayer(fTimeDelta);
	if (IsPlayerApproach(1.f))
		return FAILURE;
	else
	{
		m_pCurState = STATE_WALK;
		return RUNNING;
	}
}

NodeStates CCreeper::Attack(_float fTimeDelta)
{
	if (IsAttacking == false && IsPlayerApproach(10.f))
	{
		IsAttacking = true;
		return SUCCESS;
	}
	else if (IsAttacking == true)
	{
		c_fAccTime -= fTimeDelta;

		if (c_fAccTime < 0.f)
		{
			/*Create_Item();*/

			CFreeCamera* camera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));
			camera->Set_ShakeCamera(0.3f, 0.2f, false);
			m_pParticleManager->Create_Particle(0, m_pTransformCom->Get_State(CTransform::STATE_POSITION),nullptr);
			m_pParticleManager->Create_Particle(1, m_pTransformCom->Get_State(CTransform::STATE_POSITION),nullptr);
			_float random = RandomFloat(0.f, 90.f);
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
			CParticleManager::GetInstance()->Create_Particle(11, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), random);
			CParticleManager::GetInstance()->Create_Particle(11, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), random - 90.f);
			if (XMVectorGetX(XMVector3Length(
				XMVectorSubtract(PlayerPos(), m_pTransformCom->Get_State(CTransform::STATE_POSITION)))) < 1.5f)
			{
				static_cast<CPlayer*>(m_pPlayer)->Set_Damage(20.f);
				static_cast<CPlayer*>(m_pPlayer)->Set_Hit(true, true);
			}
			m_pStatus->fHp = 0.f;
			//CParticleManager::GetInstance()->Create_Particle(37, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			m_pGameInstance->Erase(this);
		}
		m_bIsDetected = true;
		LookAtPlayer();
		if (m_pCurState != STATE_HIT && !IsPlayerApproach(0.9f))
		{
			m_pCurState = STATE_WALK;
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		}
		
		if (m_bIsPicked) {
			c_redLightDuration -= fTimeDelta; // 빨간 불 시간 감소
			if (c_redLightDuration <= 0.0f) { // 빨간 불 시간 종료
				m_bIsPicked = false; // 빨간 불 끔
				c_blinkTimer = 0.0f; // 깜빡임 타이머 초기화
			}
		}
		else { // 깜빡임 상태
			c_blinkTimer += fTimeDelta; // 경과 시간 추가
			// 깜빡임 간격이 되면 깜빡임 상태 변경
			if (c_blinkTimer >= c_blinkInterval) {
				m_bIsPicked = true; // 빨간 불 켬
				c_blinkTimer = 0.0f; // 깜빡임 타이머 초기화
				// 깜빡임 간격을 줄임
				c_blinkInterval -= fTimeDelta * 2.5f; // 예시로 0.05초마다 간격을 줄임
				if (c_blinkInterval < 0.05f) // 간격이 너무 작아지면
				{
					c_blinkInterval = 0.05f; // 최소 간격으로 설정
				}
			}

		}

		return SUCCESS;
	}
	else
		return COOLING;
}

NodeStates CCreeper::IDLE(_float fTimeDelta)
{
	if (IsAttacking == true)
		return FAILURE;
	else
	{
		m_pCurState = STATE_IDLE;
		return SUCCESS;
	}
}

NodeStates CCreeper::Hit(_float fTimeDelta)
{
	if (m_IsHit && m_pCurState!= STATE_HIT)
	{
		m_pCurState = STATE_HIT;
		m_pTransformCom->Set_JumpState(true);
		return RUNNING;
	}

	if (m_pCurState == STATE_HIT)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			if (checkCondition())
				m_pCurState = STATE_STUN;

			return FAILURE;
		}
		else
		{
			return RUNNING;
		}
	}
	else
		return FAILURE;

}

NodeStates CCreeper::Stun(_float fTimeDelta)
{
	if (m_pCurState == STATE_STUN)
	{
		if (m_pModelCom->Get_AnimFinished())
			return FAILURE;
		else
			return RUNNING;
	}
	else
		return FAILURE;
}

NodeStates CCreeper::Dead(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f)
	{
		g_Threshold += fTimeDelta;
		m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 2.f);
		

		if (g_Threshold > 1.f)
		{
			g_Threshold = 1.f;
			RandomDrop();
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
		return FAILURE;
}

CCreeper * CCreeper::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCreeper*		pInstance = new CCreeper(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Creeper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCreeper::Clone(void * pArg)
{
	CCreeper*		pInstance = new CCreeper(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Creeper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCreeper::Free()
{
	__super::Free();
}
