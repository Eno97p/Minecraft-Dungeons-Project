#include "stdafx.h"
#include "Arrow.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "ParticleManager.h"

_float Volume = 0.3f;
CArrow::CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

CArrow::CArrow(const CArrow& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CArrow::Initialize(void* pArg)
{
	((ArrowDesc*)pArg)->fSpeedPerSec = 15.f;
	//((ArrowDesc*)pArg)->fRotationPerSec = 2.f;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vDir = ((ArrowDesc*)pArg)->vDir;

	m_vPos = ((ArrowDesc*)pArg)->vPos;
	m_vPos.y += 0.5f;


	float angle = XMVectorGetX(XMVector3AngleBetweenNormals(XMLoadFloat4(&m_vDir), m_pTransformCom->Get_State(CTransform::STATE_LOOK)));

	m_pTransformCom->Rotation_Z(m_pTransformCom->Get_State(CTransform::STATE_UP), angle);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos));
	
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat4(&m_vDir));



	m_bType = ((ArrowDesc*)pArg)->bTypeFrendly;
	ShootSound();
	if (FAILED(Add_Components()))
		return E_FAIL;

	
	return S_OK;
}

void CArrow::Priority_Tick(_float fTimeDelta)
{

}

void CArrow::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0.f)
		m_pGameInstance->Erase(this);


	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix());


	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));
	m_pTransformCom->Go_Straight(fTimeDelta);

	

	
	


}

void CArrow::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
		CParticleManager::GetInstance()->Create_Particle(5, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	}
	Collision();
}

HRESULT CArrow::Render()
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

HRESULT CArrow::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Arrow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	//CBounding_AABB::AABB_DESC aabbdesc = {};
	//aabbdesc.vExtents = _float3(0.3f, 0.2f, 0.5f);
	//aabbdesc.vCenter = _float3(0.f, aabbdesc.vExtents.y, 0.f);

	CBounding_Sphere::SPHERE_DESC spheredesc = {};
	spheredesc.fRadius = 0.5f;
	spheredesc.vCenter = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &spheredesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CArrow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

void CArrow::ShootSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowShoot-001_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowShoot-002_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowShoot-003_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	}
}

void CArrow::HitSound()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowHitLovika-002_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowHitLovika-003_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_bowHitLovika-001_soundWave.ogg"), SOUND_EFFECT, Volume);
		break;
	}
}

void CArrow::Collision()
{
	if (m_bType == true) //플레이어가 쏜 화살
	{
		list<class CGameObject*> monster = m_pGameInstance->Get_Layer_Objects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		for (auto& iter : monster)
		{
			if (static_cast<CMonster*>(iter)->InterSectFromWeapon(m_pColliderCom))
			{
				CParticleManager::GetInstance()->Create_Particle(6, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
				HitSound();
				m_pGameInstance->Erase(this);
			}
		}
	}
	else //몬스터가 쏜 화살
	{
		CGameObject* pPlayer = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
		if (static_cast<CPlayer*>(pPlayer)->Intersect(m_pColliderCom))
		{
			CParticleManager::GetInstance()->Create_Particle(6, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			HitSound();
			static_cast<CPlayer*>(pPlayer)->Set_Damage(4.f);
			m_pGameInstance->Erase(this);
		}
	}
}

CArrow* CArrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CArrow* pInstance = new CArrow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CArrow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CArrow::Clone(void* pArg)
{
	CArrow* pInstance = new CArrow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CArrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CArrow::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	__super::Free();
}

