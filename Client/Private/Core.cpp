#include "stdafx.h"
#include "Core.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "ParticleManager.h"
CCore::CCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CCore::CCore(const CCore& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CCore::Initialize(void* pArg)
{
	((CoreDesc*)pArg)->fSpeedPerSec = 5.f;
	((CoreDesc*)pArg)->fRotationPerSec = XMConvertToRadians(360.f);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vDir = ((CoreDesc*)pArg)->vDir;
	m_vPos = ((CoreDesc*)pArg)->vPos;
	m_vColor = ((CoreDesc*)pArg)->vColor;
	m_ParticleNum = ((CoreDesc*)pArg)->ParticleNum;
	m_fSpeed = ((CoreDesc*)pArg)->fSpeed;


	m_vPos.y += 0.5f;
	m_fLifeTime = 3.f;
	m_vRandomAxis = { RandomSign()* RandomFloat(0.f,1.f),RandomSign() * RandomFloat(0.f,1.f), RandomSign() * RandomFloat(0.f,1.f) };
	//m_vRandomAxis = { }
	//완전 랜덤한 축
	m_pTransformCom->Set_Scale(((CoreDesc*)pArg)->fSize, ((CoreDesc*)pArg)->fSize, ((CoreDesc*)pArg)->fSize);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos));
	
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&m_vDir));
	
	m_pTarget = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");




	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CCore::Priority_Tick(_float fTimeDelta)
{

}

void CCore::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if(m_ParticleNum == 0)
		CParticleManager::GetInstance()->Create_Particle(17, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	else if(m_ParticleNum == 1)
		CParticleManager::GetInstance()->Create_Particle(41, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);

	if (m_fLifeTime < 0.f || InterSected(m_pTarget))
	{
		_vector pPos = static_cast<CPlayer*>(m_pTarget)->Get_Pos();

		_float random = RandomFloat(0.f, 90.f);
		switch (m_ParticleNum)
		{
		case 0:
			CParticleManager::GetInstance()->Create_Particle(16, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			CParticleManager::GetInstance()->Create_Particle(1, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			CParticleManager::GetInstance()->Create_Particle(11, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), random);
			CParticleManager::GetInstance()->Create_Particle(11, m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_State(CTransform::STATE_LOOK), random-90.f);
			if (XMVectorGetX(XMVector3Length(pPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < 2.f)
			{
				static_cast<CPlayer*>(m_pTarget)->Set_Damage(35.f);
				
				static_cast<CPlayer*>(m_pTarget)->Set_Hit(true, true);
			}
			m_pGameInstance->PlaySound_Z(_T("explosion1.mp3"), SOUND_EFFECT, 0.3f);
			break;
		case 1:
			CParticleManager::GetInstance()->Create_Particle(42, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			CParticleManager::GetInstance()->Create_Particle(43, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			if (XMVectorGetX(XMVector3Length(pPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < 1.f)
			{
				static_cast<CPlayer*>(m_pTarget)->Set_Damage(20.f);
				static_cast<CPlayer*>(m_pTarget)->Set_Hit(true, true);
			}
			m_pGameInstance->PlaySound_Z(_T("sfx_boss_ArchillagerMagicImpact-001.ogg"), SOUND_EFFECT, 0.3f);
			break;
		}

		m_pGameInstance->Erase(this);
	}
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	
	m_pTransformCom->MoveDirection(XMLoadFloat4(&m_vDir), fTimeDelta * m_fSpeed);
	m_pTransformCom->Turn(XMLoadFloat3(&m_vRandomAxis), fTimeDelta);
}

void CCore::Late_Tick(_float fTimeDelta)
{
	/*Collision();*/
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CCore::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

HRESULT CCore::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_CubeTexture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	CBounding_Sphere::SPHERE_DESC spheredesc = {};
	spheredesc.fRadius = 0.3f;
	spheredesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &spheredesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCore::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CCore::Collision()
{
	CGameObject* pPlayer = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
	if (static_cast<CPlayer*>(pPlayer)->Intersect(m_pColliderCom))
		m_pGameInstance->Erase(this);
	
}

CCore* CCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCore* pInstance = new CCore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CCore");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCore::Clone(void* pArg)
{
	CCore* pInstance = new CCore(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CCore");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCore::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	__super::Free();
}

