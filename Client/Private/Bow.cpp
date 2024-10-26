#include "stdafx.h"
#include "Bow.h"
#include "Player.h"
#include "GameInstance.h"

CBow::CBow(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CWeapon{ pDevice, pContext }
{
}

CBow::CBow(const CBow & rhs)
	: CWeapon{ rhs }
{
}

HRESULT CBow::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBow::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	m_pTransformCom->Rotation_Z(vRight, XMConvertToRadians(90.f));
	m_pTransformCom->Rotation_Z(vLook, XMConvertToRadians(90.f));

	
	return S_OK;
}

void CBow::Priority_Tick(_float fTimeDelta)
{
}

void CBow::Tick(_float fTimeDelta)
{



}

void CBow::Late_Tick(_float fTimeDelta)
{
	Socket_Matrix_Bind();
	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

	if (true == m_pGameInstance->isIn_WorldFrustum(Get_Part_Pos(), 2.f))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);

	}
}

HRESULT CBow::Render()
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

HRESULT CBow::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bow"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	CBounding_OBB::OBB_DESC		BoundingDesc{};

	BoundingDesc.vExtents = _float3(0.1f, -0.2f, 0.1f);
	BoundingDesc.vCenter = _float3(0.f, BoundingDesc.vExtents.y, 0.f);
	BoundingDesc.vRadians = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &BoundingDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBow::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	return S_OK;
}

CBow * CBow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBow*		pInstance = new CBow(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBow::Clone(void * pArg)
{
	CBow*		pInstance = new CBow(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBow::Free()
{
	__super::Free();

}
