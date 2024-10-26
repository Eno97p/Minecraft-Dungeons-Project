#include "stdafx.h"
#include "Enderman.h"

#include "GameInstance.h"

CEnderman::CEnderman(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CEnderman::CEnderman(const CEnderman & rhs)
	: CMonster{ rhs }
{
}

HRESULT CEnderman::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnderman::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Enderman");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(4, true));

	if (FAILED(Add_Nodes()))
		return E_FAIL;

	return S_OK;
}

void CEnderman::Priority_Tick(_float fTimeDelta)
{
}

void CEnderman::Tick(_float fTimeDelta)
{



	m_ComBehavior->Update(fTimeDelta);
	__super::Tick(fTimeDelta);
	/*m_pModelCom->Play_Animation(fTimeDelta);*/
	SetAnimIndex(fTimeDelta);

	if (m_pCurState == STATE_RUN || m_pCurState == STATE_WALK)
		m_pTransformCom->Go_Straight(fTimeDelta, m_pNavigationCom);
	
	
}

void CEnderman::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		CollisionEachOther(fTimeDelta);
	}
}

HRESULT CEnderman::Render()
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
	m_pNavigationCom->Render();
#endif

	return S_OK;
}

HRESULT CEnderman::SetStatus()
{
	MonsterStatus* Stat = new MonsterStatus;
	Stat->fDamage = 20.f;
	Stat->fHp = 100.f;
	Stat->fMaxHp = 200.f;
	Stat->m_fRotationpersec = XMConvertToRadians(50.f);
	Stat->m_fSpeed = 3.f;
	m_pStatus = Stat;
	return S_OK;
}

HRESULT CEnderman::Add_Components()
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

	CNavigation::NAVIGATION_DESC	NavigationDesc{};

	NavigationDesc.iCurrentCellIndex = 0;
	NavigationDesc.vPos = &m_MonsterDec.Pos;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom),&NavigationDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnderman::Bind_ShaderResources()
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

void CEnderman::SetAnimIndex(_float fTimeDelta)
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
	case STATE_ATTACK:
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 5;
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

HRESULT CEnderman::Add_Nodes()
{
	//m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	//m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	//m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Hit"), CBehaviorTree::Action , bind(&CEnderman::Hit, this));
	//m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Stun"), CBehaviorTree::Action, bind(&CEnderman::Stun, this));


	//m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("DetectSequence"), CBehaviorTree::Sequence);
	//m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("Detect"), CBehaviorTree::Action, bind(&CEnderman::Detect, this));
	//m_ComBehavior->Add_Node(TEXT("DetectSequence"), TEXT("DetectSelector"), CBehaviorTree::Selector);
	//m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("MoveToPlayer"), CBehaviorTree::Action, bind(&CEnderman::MoveToPlayer, this));
	//m_ComBehavior->Add_Node(TEXT("DetectSelector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CEnderman::Attack, this));
	//m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLESelector"), CBehaviorTree::Selector);


	//m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("Patroll"), CBehaviorTree::Action, bind(&CEnderman::Patroll, this));
	//m_ComBehavior->Add_Node(TEXT("IDLESelector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CEnderman::IDLE, this));

	return S_OK;
}

NodeStates CEnderman::Patroll(_float fTimeDelta)
{
	if (m_bIdle == false)
	{
		if (m_pCurState != STATE_WALK)
		{
			
			m_pCurState = STATE_WALK;
			return RUNNING;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;
}


NodeStates CEnderman::MoveToPlayer()
{
	LookAtPlayer();
	if (IsPlayerApproach(2.f))
		return FAILURE;
	else
	{
		if (IsAttacking)
			return FAILURE;
		else
		{
			m_pCurState = STATE_RUN;
			return RUNNING;
		}
	}
}

NodeStates CEnderman::Attack()
{
	if (IsAttacking = false)
	{
		m_pCurState = STATE_ATTACK;
		IsAttacking = true;
		return RUNNING;
	}
	else
	{
		if (!m_pModelCom->Get_AnimFinished())
			return RUNNING;
		else
		{
			IsAttacking = false;
			return FAILURE;
		}
	}
}

NodeStates CEnderman::IDLE()
{
	m_pCurState = STATE_IDLE;
	return SUCCESS;
}

NodeStates CEnderman::Hit()
{
	if (m_IsHit && m_pCurState!= STATE_HIT)
	{
		m_pCurState = STATE_HIT;
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
			return RUNNING;
	}
	else
		return FAILURE;

}

NodeStates CEnderman::Stun()
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

CEnderman * CEnderman::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnderman*		pInstance = new CEnderman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Creeper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnderman::Clone(void * pArg)
{
	CEnderman*		pInstance = new CEnderman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Creeper");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnderman::Free()
{
	__super::Free();
}
