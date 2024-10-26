#include "stdafx.h"
#include "Skeleton.h"

#include "GameInstance.h"
#include "Weapon.h"
#include "Arrow.h"

CSkeleton::CSkeleton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CSkeleton::CSkeleton(const CSkeleton & rhs)
	: CMonster{ rhs }
{
}

HRESULT CSkeleton::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkeleton::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(AddPartsObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	Set_Unique_ID("Skeleton");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(0, true));
	m_pTransformCom->Set_Power(3.f);
	return S_OK;
}

void CSkeleton::Priority_Tick(_float fTimeDelta)
{
	m_Bow->Priority_Tick(fTimeDelta);

	if (m_firsthit)
		m_SmallHpBar->Priority_Tick(fTimeDelta);
}

void CSkeleton::Tick(_float fTimeDelta)
{

	m_ComBehavior->Update(fTimeDelta);
	__super::Tick(fTimeDelta);
	SetAnimIndex(fTimeDelta);

	m_Bow->Tick(fTimeDelta);

	/*if (m_pCurState == STATE_RUN || m_pCurState == STATE_WALK)*/
	

	/*if (m_pCurState == STATE_ESCAPE)*/
		

	//if (m_pCurState == STATE_HIT)
	//{
	//	
	//}
	if (m_firsthit)
		m_SmallHpBar->Tick(fTimeDelta);
}

void CSkeleton::Late_Tick(_float fTimeDelta)
{
	if (m_firsthit)
		m_SmallHpBar->Late_Tick(fTimeDelta);

	
	m_Bow->Late_Tick(fTimeDelta);

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

HRESULT CSkeleton::Render()
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

HRESULT CSkeleton::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 30.f;
	Stat->fMaxHp = 30.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 2.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CSkeleton::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton"),
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

HRESULT CSkeleton::AddPartsObjects()
{
	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;
	m_Bow = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Bow"), &WeaponDesc);
	if (nullptr == m_Bow)
		return E_FAIL;

	return S_OK;
}

HRESULT CSkeleton::Bind_ShaderResources()
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

HRESULT CSkeleton::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);

	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Dead"), CBehaviorTree::Action, bind(&CSkeleton::Dead, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Hit"), CBehaviorTree::Action, bind(&CSkeleton::Hit, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Stun"), CBehaviorTree::Action, bind(&CSkeleton::Stun, this, std::placeholders::_1));


	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CSkeleton::Detect, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("DetectSelector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CSkeleton::Attack, this, std::placeholders::_1), CoolDown, 3.f);
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CSkeleton::MoveToPlayer, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("EscapeFromPlayer"), CBehaviorTree::Action, bind(&CSkeleton::EscapeFromplayer, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Hold"), CBehaviorTree::Action, bind(&CSkeleton::Hold, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLESelector"), CBehaviorTree::Selector);


	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CSkeleton::Patroll, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CSkeleton::IDLE, this, std::placeholders::_1));
	return S_OK;
}

NodeStates CSkeleton::Patroll(_float fTimeDelta)
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

void CSkeleton::Shoot_Arrow()
{
	CArrow::ArrowDesc Arrowdec = {};
	Arrowdec.bTypeFrendly = false;

	_vector vDir = Calculate_Player_Dir();
	XMStoreFloat4(&Arrowdec.vDir, vDir);
	XMStoreFloat4(&Arrowdec.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Arrowdec);
}

void CSkeleton::SetAnimIndex(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{ 2, true };
	switch (m_pCurState)
	{
	case STATE_IDLE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 2;
		break;
	case STATE_ESCAPE:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_RUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_WALK:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = true;
		AnimDesc.iAnimIndex = 5;
		break;
	case STATE_NOEVENT:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 3;
		break;
	case STATE_ATTACK:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 0;
		break;
	case STATE_STUN:
		m_Animspeed = 1.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 4;
		break;
	case STATE_HIT:
		m_Animspeed = 2.f;
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 1;
		break;
	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta* m_Animspeed);


}

NodeStates CSkeleton::MoveToPlayer(_float fTimeDelta)
{
	LookAtPlayer();
	if (IsAttacking)
		return FAILURE;


	if (IsPlayerApproach(6.f))		//6보다 안에 있을때
	{
		m_pCurState = STATE_ESCAPE;
		return FAILURE;
	}
	else if (!IsPlayerApproach(8.f) && IsPlayerApproach(10.f))		//8보다 밖에 있을때
	{
		m_pCurState = STATE_RUN;
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
		return RUNNING;
	}
	else
	{
		/*m_pCurState = STATE_IDLE;*/
		return FAILURE;
	}

		




	//else
	//{
	//	if (IsAttacking)
	//		return FAILURE;
	//	else
	//	{
	//		m_pCurState = STATE_RUN;
	//		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	//		return RUNNING;
	//	}
	//}
}

NodeStates CSkeleton::EscapeFromplayer(_float fTimeDelta)
{
	if (m_pCurState != STATE_ESCAPE)
		return FAILURE;
	else
	{
		if (IsAttacking)
			return FAILURE;
		else
		{
			m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}



	//if (IsPlayerApproach(6.f))
	//{
	//	if (IsAttacking)
	//		return FAILURE;
	//	else
	//	{
	//		m_pCurState = STATE_ESCAPE;
	//		m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
	//		return RUNNING;
	//	}
	//}
	//else
	//{
	//	m_pCurState = STATE_IDLE;
	//	return FAILURE;
	//}
}

NodeStates CSkeleton::Attack(_float fTimeDelta)
{
	if (IsAttacking == false)
	{
		m_pCurState = STATE_ATTACK;
		IsAttacking = true;
		LookAtPlayer();
		Shoot_Arrow();
		return RUNNING;
	}
	else
	{
		if (!m_pModelCom->Get_AnimFinished())
			return RUNNING;
		else
		{
			IsAttacking = false;
			return SUCCESS;
		}
	}
}

NodeStates CSkeleton::IDLE(_float fTimeDelta)
{

	if (IsAttacking)
	{
		IsAttacking = false;
		return FAILURE;
	}
	else
	{
		m_pCurState = STATE_IDLE;
		return RUNNING;
	}
}

NodeStates CSkeleton::Hit(_float fTimeDelta)
{
	if (m_IsHit && m_pCurState != STATE_HIT)
	{
		m_pCurState = STATE_HIT;
		HurtSound();
		m_pTransformCom->Set_JumpState(true);
		return RUNNING;
	}

	if (m_pCurState == STATE_HIT)
	{
		if (m_pModelCom->Get_AnimFinished())
		{
			return FAILURE;
		}
		else
		{
			m_pTransformCom->Get_Pushed(Calculate_Player_Dir(), fTimeDelta, m_pNavigationCom);
			return RUNNING;
		}
	}
	else
		return FAILURE;
}

NodeStates CSkeleton::Stun(_float fTimeDelta)
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

NodeStates CSkeleton::Hold(_float fTimeDelta)
{
	LookAtPlayer();
	m_pCurState = STATE_IDLE;
	return RUNNING;
}

NodeStates CSkeleton::Dead(_float fTimeDelta)
{
	if (m_pStatus->fHp <= 0.f)
	{
		m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fTimeDelta * 2.f);
		g_Threshold += fTimeDelta;
		if (g_Threshold > 1.f)
		{
			g_Threshold = 1.f;
			RandomDrop();
			DeathSound();
			m_pGameInstance->Erase(this);
		}
		return RUNNING;
	}
	else
		return FAILURE;
}

void CSkeleton::HurtSound()
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

void CSkeleton::DeathSound()
{
	_int RandomSound = RandomInt(0, 6);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-005_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 5:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-006_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 6:
		m_pGameInstance->PlaySound_Z(_T("sfx_mob_skeletonDeath-007_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}

CSkeleton * CSkeleton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkeleton*		pInstance = new CSkeleton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkeleton::Clone(void * pArg)
{
	CSkeleton*		pInstance = new CSkeleton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkeleton::Free()
{
	__super::Free();
	Safe_Release(m_Bow);

}
