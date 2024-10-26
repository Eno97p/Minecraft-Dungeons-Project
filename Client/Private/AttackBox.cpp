#include "stdafx.h"
#include "AttackBox.h"

#include "GameInstance.h"

CAttackBox::CAttackBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CAttackBox::CAttackBox(const CAttackBox & rhs)
	: CPartObject{ rhs }
{
}

_bool CAttackBox::InterSect(CCollider* SrcCol)
{
	return m_pColliderCom->Intersect(SrcCol);
}

_bool CAttackBox::InterSected(CCollider* SrcCol)
{
	return SrcCol->Intersect(m_pColliderCom);
}

HRESULT CAttackBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAttackBox::Initialize(void* pArg)
{
	ATTACKBOX_DESC* pDesc = (ATTACKBOX_DESC*)pArg;
	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;
	m_vCenter = pDesc->vCenter;
	m_vRadius = pDesc->vRadius;
	m_vExtents = pDesc->vExtents;
	m_eType = pDesc->eType;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if(FAILED(Add_Components(m_eType)))
		return E_FAIL;

	return S_OK;
}

void CAttackBox::Priority_Tick(_float fTimeDelta)
{
}

void CAttackBox::Tick(_float fTimeDelta)
{
}

void CAttackBox::Late_Tick(_float fTimeDelta)
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));

	m_pColliderCom->Tick(XMLoadFloat4x4(&m_WorldMatrix));

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
	//m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CAttackBox::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif
	return S_OK;
}

_float4x4* CAttackBox::Get_ATB_Mat()
{
	return &m_WorldMatrix;
}

_vector CAttackBox::Get_ATB_Pos()
{
	return XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43,1.f);
}

HRESULT CAttackBox::Add_Components(CCollider::TYPE ColType)
{
	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = m_vExtents;
	aabbdesc.vCenter = m_vCenter;

	CBounding_Sphere::SPHERE_DESC spheredesc = {};
	spheredesc.fRadius = m_vRadius;
	spheredesc.vCenter = m_vCenter;


	switch (ColType)
	{
	case CCollider::TYPE_AABB:
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbdesc)))
			return E_FAIL;
		break;
	case CCollider::TYPE_OBB:
		//지원안함
		return E_FAIL;
		break;
	case CCollider::TYPE_SPHERE:
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &spheredesc)))
			return E_FAIL;
		break;

	}

	return S_OK;
}


CAttackBox* CAttackBox::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAttackBox* pInstance = new CAttackBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CAttackBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAttackBox::Clone(void* pArg)
{
	CAttackBox* pInstance = new CAttackBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CAttackBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAttackBox::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
