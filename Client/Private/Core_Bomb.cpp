#include "stdafx.h"
#include "Core_Bomb.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "ParticleManager.h"
CCore_Bomb::CCore_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CCore_Bomb::CCore_Bomb(const CCore_Bomb& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCore_Bomb::Initialize(void* pArg)
{
	((CoreBombDesc*)pArg)->fSpeedPerSec = RandomFloat(4.f,6.f);
	((CoreBombDesc*)pArg)->fRotationPerSec = XMConvertToRadians(360.f);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_pTarger = ((CoreBombDesc*)pArg)->pTarget;
	m_pTargetTransform = static_cast<CTransform*>(((CoreBombDesc*)pArg)->pTarget->Get_Component(TEXT("Com_Transform")));
	

	m_vPos = ((CoreBombDesc*)pArg)->vPos;
	m_vPos.y += 0.5f;
	m_fLifeTime = 6.f;
	m_vRandomAxis = { RandomSign()* RandomFloat(0.f,1.f),RandomSign() * RandomFloat(0.f,1.f), RandomSign() * RandomFloat(0.f,1.f) };
	//m_vRandomAxis = { }
	//완전 랜덤한 축

	m_pTransformCom->Set_Scale(1.5f, 1.5f, 1.5f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos));

	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&m_vPos), nullptr);


	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCore_Bomb::Priority_Tick(_float fTimeDelta)
{

}

void CCore_Bomb::Tick(_float fTimeDelta)
{


	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0.f || m_pTarger->InterSected(this))
	{
		if (m_pTarger->InterSected(this))
		{
			static_cast<CPlayer*>(m_pTarger)->Set_Damage(40.f);
			static_cast<CPlayer*>(m_pTarger)->Set_Hit(true, true);
		}


		_float RandomAxis = RandomFloat(0.f, 180.f);
		_int RandomSignInt = RandomSign();

		CParticleManager::GetInstance()->Create_Particle(44, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		if (RandomSignInt > 0)
		{
			CParticleManager::GetInstance()->Create_Particle(45, 
				m_pTransformCom->Get_State(CTransform::STATE_POSITION), 
				m_pTransformCom->Get_State(CTransform::STATE_LOOK), RandomAxis);
			CParticleManager::GetInstance()->Create_Particle(45,
				m_pTransformCom->Get_State(CTransform::STATE_POSITION),
				m_pTransformCom->Get_State(CTransform::STATE_LOOK), RandomAxis-90.f);
		}
		else
		{
			CParticleManager::GetInstance()->Create_Particle(45,
				m_pTransformCom->Get_State(CTransform::STATE_POSITION),
				m_pTransformCom->Get_State(CTransform::STATE_RIGHT), RandomAxis);
			CParticleManager::GetInstance()->Create_Particle(45,
				m_pTransformCom->Get_State(CTransform::STATE_POSITION),
				m_pTransformCom->Get_State(CTransform::STATE_RIGHT), RandomAxis -90.f);
		}
		m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerMagicImpact-001.ogg"), SOUND_EFFECT, 0.3f);
		m_pGameInstance->Erase(this);
	}

	CParticleManager::GetInstance()->Create_Particle(41, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	

	_vector TargetPos = m_pTargetTransform->Get_State(CTransform::STATE_POSITION);
	_vector vDirection = XMVector3Normalize(XMVectorSubtract(TargetPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	m_pTransformCom->MoveDirection(vDirection, fTimeDelta);
	m_pTransformCom->Turn(XMLoadFloat3(&m_vRandomAxis), fTimeDelta);
}

void CCore_Bomb::Late_Tick(_float fTimeDelta)
{
	/*Collision();*/
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CCore_Bomb::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CCore_Bomb::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Core"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	CBounding_Sphere::SPHERE_DESC spheredesc = {};
	spheredesc.fRadius = 0.4f;
	spheredesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &spheredesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCore_Bomb::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CCore_Bomb::Collision()
{
	CGameObject* pPlayer = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
	if (static_cast<CPlayer*>(pPlayer)->Intersect(m_pColliderCom))
		m_pGameInstance->Erase(this);
	
}

CCore_Bomb* CCore_Bomb::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCore_Bomb* pInstance = new CCore_Bomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCore_Bomb");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCore_Bomb::Clone(void* pArg)
{
	CCore_Bomb* pInstance = new CCore_Bomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCore_Bomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCore_Bomb::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	__super::Free();
}

