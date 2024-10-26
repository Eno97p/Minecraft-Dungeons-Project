#include "Map.h"

#include "GameInstance.h"

CMap::CMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMap::CMap(const CMap & rhs)
	: CGameObject{ rhs }
{
}

_bool CMap::Mesh_Picking(_float3* PickingPoint)
{
	return m_pModelCom->Picking(m_pTransformCom, PickingPoint);
}


void CMap::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
}
