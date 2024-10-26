#include "stdafx.h"
#include "Door.h"

#include "GameInstance.h"
#include "ParticleManager.h"
#include "FreeCamera.h"

_int CDoor::m_iCount = 0;

CDoor::CDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment{ pDevice, pContext }
{
}

CDoor::CDoor(const CDoor & rhs)
	: CEnvironment{ rhs }
{
}

HRESULT CDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDoor::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Door");


	m_pCamera = static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"));
	return S_OK;
}

void CDoor::Priority_Tick(_float fTimeDelta)
{
}

void CDoor::Tick(_float fTimeDelta)
{

	__super::Tick(fTimeDelta);

	if (isDoorOpen)
	{
		if (m_iCount == 0)
		{
			m_pCamera->LerpAngle(182.201324, fTimeDelta);
			m_pCamera->Distance_Up(fTimeDelta);
			CParticleManager::GetInstance()->Create_Particle(65, XMVectorSet(-9.77f, 18.51f, -116.90f, 1.f), nullptr);
			CParticleManager::GetInstance()->Create_Particle(65, XMVectorSet(-9.80f, 18.51f, -112.16f, 1.f), nullptr);
			
		}
		else
		{
			m_pCamera->LerpAngle(181.383789, fTimeDelta);
			m_pCamera->Distance_Up(fTimeDelta * 1.2f);
			CParticleManager::GetInstance()->Create_Particle(65, XMVectorSet(-198.93f, 45.51f, 30.62f, 1.f), nullptr);
			CParticleManager::GetInstance()->Create_Particle(65, XMVectorSet(-192.22f, 45.51f, 30.60f, 1.f), nullptr);
		}

		_vector Dir = XMVectorSet(0.f, 1.f, 0.f, 0.f) * -1;
		m_pTransformCom->MoveDirection(Dir, fTimeDelta*0.5f);
		

		if (abs(m_EnviDec.Pos.y - XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION))) > 6.f)
		{
			m_pCamera->Reset_Distance();
			m_pCamera->Set_Target(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));
			if (m_iCount == 0)
			{
				m_pGameInstance->PlayBGM(_T("SecondPhaseBGM.ogg"), 0.3f);
				Summon_Box();
			}
			m_iCount += 1;
			m_pGameInstance->Erase(this);
		}
	}
		


}

void CDoor::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

}

HRESULT CDoor::Render()
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



HRESULT CDoor::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	



	return S_OK;
}

HRESULT CDoor::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", m_bIsPicked)))
		return E_FAIL;

	return S_OK;
}

void CDoor::Summon_Box()
{
	CEnvironment::Envi_Desc eDec = {};
				
	eDec.Pos = _float4(-1.25f, 20.51f, -114.59f, 1.f);
	eDec.RotationAngle = XMConvertToRadians(-90.f);
	eDec.Scale = 1.f;

	CParticleManager::GetInstance()->Create_Particle(51, XMVectorSet(-1.25f, 20.51f, -114.59f, 1.f), nullptr);
	CGameObject* Box = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_ItemBox"), &eDec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Frendly"), Box);


			
}





CDoor * CDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDoor*		pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDoor::Clone(void * pArg)
{
	CDoor*		pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();

}
