#include "stdafx.h"
#include "Axe.h"
#include "GameInstance.h"
#include "ParticleManager.h"
#include "Player.h"

CAxe::CAxe(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon{ pDevice, pContext }
{
}

CAxe::CAxe(const CAxe & rhs)
	: CWeapon{ rhs }
{
}

HRESULT CAxe::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAxe::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaling(1.5f, 1.5f, 1.5f);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(250.f));
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	/*m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.2f, 1.f));*/

	return S_OK;
}

void CAxe::Priority_Tick(_float fTimeDelta)
{
}

void CAxe::Tick(_float fTimeDelta)
{

}

void CAxe::Late_Tick(_float fTimeDelta)
{
	Socket_Matrix_Bind();
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	//_float4 center = _float4(m_pColliderCom->Get_Center().x, m_pColliderCom->Get_Center().y, m_pColliderCom->Get_Center().z, 1.f);
	//_vector Pos = XMLoadFloat4(&center);
	if (m_pState != nullptr)
	{
		if (*m_pState == CPlayer::STATE_ATTACK)
			CParticleManager::GetInstance()->Create_Particle(73, Get_Collider_Center(), nullptr);
	}

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

}

HRESULT CAxe::Render()
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

HRESULT CAxe::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	


	CBounding_Sphere::SPHERE_DESC SDesc{};

	SDesc.vCenter = _float3(0.f, -0.4f, 0.f);
	SDesc.fRadius = 0.8f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &SDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CAxe::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CAxe * CAxe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAxe*		pInstance = new CAxe(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAxe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAxe::Clone(void * pArg)
{
	CAxe*		pInstance = new CAxe(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAxe");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAxe::Free()
{
	__super::Free();

}