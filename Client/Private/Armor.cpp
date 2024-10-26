#include "stdafx.h"
#include "Armor.h"

#include "GameInstance.h"

CArmor::CArmor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CArmor::CArmor(const CArmor & rhs)
	: CPartObject{ rhs }
{
}

HRESULT CArmor::Initialize(void* pArg)
{
	ARMORDESC* pDesc = (ARMORDESC*)pArg;

	m_pSocketMatrix = pDesc->pCombinedTransformationMatrix;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}


void CArmor::Bind_ProjResources(_float4x4* m_ViewMatrix, _float4x4* m_ProjMatrix)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return;
}

void CArmor::RenderUI()
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

void CArmor::Socket_Matrix_Bind()
{
	_matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * SocketMatrix * XMLoadFloat4x4(m_pParentMatrix));
}

void CArmor::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
