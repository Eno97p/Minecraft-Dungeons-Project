#include "stdafx.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CPartObject{ rhs }
{
}

HRESULT CWeapon::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	//_vector vDir = XMVector4Normalize(XMVectorSet(-135.f, 37.f, -62.f, 1.f) - XMVectorSet(-225.f, 150.f, -135.f, 1.f));
		/* ±¤¿ø ±âÁØÀÇ ºä º¯È¯Çà·Ä. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(68.f, 150.f, -126.f, 0.f), XMVectorSet(83.f, 24.f, -36.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

_vector CWeapon::Get_Collider_Center()
{
	_float3 vCenter3 = m_pColliderCom->Get_Center();
	_float4 vResult = { vCenter3.x,vCenter3.y ,vCenter3.z ,1.f };
	return XMLoadFloat4(&vResult);
}

HRESULT CWeapon::Initialize(void* pArg)
{
	WEAPON_DESC* pDesc = (WEAPON_DESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}


void CWeapon::Bind_ProjResources(_float4x4* m_ViewMatrix, _float4x4* m_ProjMatrix)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return;
}

void CWeapon::RenderUI()
{
	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return;

		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}
}

void CWeapon::Socket_Matrix_Bind()
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
