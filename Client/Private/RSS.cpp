#include "stdafx.h"
#include "RSS.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "ParticleManager.h"
RSS::RSS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject{ pDevice, pContext }
{
}

RSS::RSS(const RSS& rhs)
	: CGameObject{ rhs }
{
}

HRESULT RSS::Initialize(void* pArg)
{
	((RSSDESC*)pArg)->fSpeedPerSec = 5.f;
	((RSSDESC*)pArg)->fRotationPerSec = XMConvertToRadians(360.f);

	m_vColor = ((RSSDESC*)pArg)->vColor;
	m_iParticleNumber = ((RSSDESC*)pArg)->ParticleNum;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_vDir = ((RSSDESC*)pArg)->vDir;
	m_vPos = ((RSSDESC*)pArg)->vPos;
	m_vPoriginPos = m_vPos;
	m_vDir.y += RandomFloat(1.f, 1.5f);

	_float randomfloat = RandomFloat(0.1f, 0.5f);
	m_vDir.x += randomfloat * RandomSign();

	m_pTransformCom->Set_Scale(0.2f, 0.2f, 0.2f);
	m_pTransformCom->LookAt_Direction(XMLoadFloat4(&m_vDir));

	_float aRandom = RandomFloat(0.f, 30.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos));
	
	XMStoreFloat4(&m_vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));

	RandomSpeed = RandomFloat(2.f, 4.f);
	return S_OK;
}

void RSS::Priority_Tick(_float fTimeDelta)
{

}

void RSS::Tick(_float fTimeDelta)
{
	fLifeTime -= fTimeDelta;

	m_vDir.y -= fTimeDelta * 0.5f;
	m_pTransformCom->LookAt_Direction(XMLoadFloat4(&m_vDir));
	//m_pTransformCom->MoveDirection(XMLoadFloat4(&m_vDir), fTimeDelta, m_pNavigationCom);
	
	
	//if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < (m_vPoriginPos.y-2.f) || fLifeTime < 0.f)
	//{

	//}

	if (XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)) < (m_vPoriginPos.y - 1.5f))
	{
		m_pNavigationCom->Compute_Height(m_pTransformCom);
	}
	else
		m_pTransformCom->Go_Straight(fTimeDelta * RandomSpeed, m_pNavigationCom);

	if (fLifeTime < 0.f)
	{
		if (m_iParticleNumber == 0)
		{
			CParticleManager::GetInstance()->Create_Particle(10, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			CParticleManager::GetInstance()->Create_Particle(11, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			CParticleManager::GetInstance()->Create_Particle(37, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			if (XMVectorGetX(XMVector3Length(m_pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < 2.f)
			{
				m_pPlayer->Set_Hit(true, true);
				m_pPlayer->Set_Damage(20.f);
			}
			m_pGameInstance->PlaySound_Z(_T("small_explosion1.mp3"), SOUND_EFFECT, 0.3f);
		}
		else
		{
			_int aRandomSign = RandomSign();
			if (aRandomSign > 0)
			{
				CParticleManager::GetInstance()->Create_Particle(32, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
				CParticleManager::GetInstance()->Create_Particle(33, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			}
			else
			{
				CParticleManager::GetInstance()->Create_Particle(34, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
				CParticleManager::GetInstance()->Create_Particle(35, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			}
			CParticleManager::GetInstance()->Create_Particle(36, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);

			if (XMVectorGetX(XMVector3Length(m_pPlayer->Get_Pos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < 2.f)
			{
				m_pPlayer->Set_Hit(true, true);
				m_pPlayer->Set_Damage(20.f);
			}
			m_pGameInstance->PlaySound_Z(_T("small_explosion1.mp3"), SOUND_EFFECT, 0.2f);
		}
		m_pGameInstance->Erase(this);

	}



	m_fDuration -= fTimeDelta;
	if (m_fDuration < 0.f)
	{
		m_fRatio = 1.f;
		m_fDuration = 0.3f;
		IsBlur = true;
	}


	if (m_fRatio > 0.0f)
	{
		m_fRatio -= fTimeDelta / 0.2f;
		if (m_fRatio < 0.0f) {
			m_fRatio = 0.0f;
			IsBlur = false;
		}
	}

}

void RSS::Late_Tick(_float fTimeDelta)
{
	if(IsBlur)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT RSS::Render()
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


	return S_OK;
}

HRESULT RSS::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_CubeTexture"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	CNavigation::NAVIGATION_DESC	NavigationDesc{};

	NavigationDesc.iCurrentCellIndex = 0;
	NavigationDesc.vPos = &m_vPos;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT RSS::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}


RSS* RSS::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	RSS* pInstance = new RSS(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : RSS");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* RSS::Clone(void* pArg)
{
	RSS* pInstance = new RSS(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : RSS");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void RSS::Free()
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	__super::Free();
}

