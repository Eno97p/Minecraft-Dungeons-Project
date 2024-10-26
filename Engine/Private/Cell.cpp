#include "..\Public\Cell.h"
#include "VIBuffer_Cell.h"
#include "VIBuffer_Cell_Pick.h"
CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_vector CCell::Get_Center_Point()
{
	_float sumX = m_vPoints[0].x + m_vPoints[1].x + m_vPoints[2].x;
	_float sumY = m_vPoints[0].y + m_vPoints[1].y + m_vPoints[2].y;
	_float sumZ = m_vPoints[0].z + m_vPoints[1].z + m_vPoints[2].z;

	_float avgX = sumX / 3.0f;
	_float avgY = sumY / 3.0f;
	_float avgZ = sumZ / 3.0f;
	

	return XMVectorSet(avgX, avgY, avgZ, 1.f);
}

HRESULT CCell::Save_Cells(ofstream& os)
{
	os.write((char*)m_vPoints, sizeof(_float3) * POINT_END);
	os.write((char*)&m_iIndex, sizeof(_int));
	os.write((char*)m_iNeighborIndices, sizeof(_int)* LINE_END);
	return S_OK;
}

HRESULT CCell::Load_Cells(ifstream& is)
{
	is.read((char*)m_vPoints, sizeof(_float3) * POINT_END);
	is.read((char*)&m_iIndex, sizeof(_int));
	is.read((char*)m_iNeighborIndices, sizeof(_int) * LINE_END);
#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pVIBuffer_Picked = CVIBuffer_Cell_Pick::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer_Picked)
		return E_FAIL;

#endif
	return S_OK;
}

_float CCell::Compute_Height(_float3 vLocalpos)
{
	_vector vPlane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]),
		XMLoadFloat3(&m_vPoints[1]),
		XMLoadFloat3(&m_vPoints[2]));

	return (-XMVectorGetX(vPlane) * vLocalpos.x - XMVectorGetZ(vPlane) * vLocalpos.z - XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	m_iIndex = iIndex;

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

#ifdef _DEBUG
	m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;
	m_pVIBuffer_Picked = CVIBuffer_Cell_Pick::Create(m_pDevice, m_pContext, m_vPoints);
	if (nullptr == m_pVIBuffer_Picked)
		return E_FAIL;

#endif

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vSourPoint, _fvector vDestPoint)
{
	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint))
			return true;
	}

	if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vSourPoint))
	{
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint))
			return true;
		if (true == XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPoint, _int* pNeighborIndex)
{
	_vector		vSour, vDest;
	for (size_t i = 0; i < LINE_END; i++)
	{
		vSour = vPoint - XMLoadFloat3(&m_vPoints[i]);

		_vector		vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSour), XMVector3Normalize(vDest))))
		{
			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}

_bool CCell::isIn(_fvector vPoint, _int* pNeighborIndex, _float4* LineDir)
{
	_vector      vSour, vDest;
	for (size_t i = 0; i < LINE_END; i++)
	{
		vSour = vPoint - XMLoadFloat3(&m_vPoints[i]);

		_vector      vLine = XMLoadFloat3(&m_vPoints[(i + 1) % POINT_END]) - XMLoadFloat3(&m_vPoints[i]);
		//2차원 외적
		vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);
		_vector Nordir = XMVector3Normalize(vDest);
		_float a = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSour), XMVector3Normalize(vDest)));

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vSour), XMVector3Normalize(vDest))))
		{
			if (nullptr != LineDir)
				XMStoreFloat4(LineDir, XMVector3Normalize(vDest));//충돌선의 노멀벡터 저장

			*pNeighborIndex = m_iNeighborIndices[i];
			return false;
		}
	}

	return true;
}


#ifdef _DEBUG
HRESULT CCell::Render()
{
	m_pVIBuffer->Bind_Buffers();
	m_pVIBuffer->Render();

	if (isPicked)
	{
		m_pVIBuffer_Picked->Bind_Buffers();
		m_pVIBuffer_Picked->Render();
	}
	
	return S_OK;
}
#endif

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Created : CCell");
		return nullptr;
	}

	return pInstance;
}

CCell* CCell::LoadCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& is)
{
	CCell* pInstance = new CCell(pDevice, pContext);
	if (FAILED(pInstance->Load_Cells(is)))
	{
		MSG_BOX("Failed to Load Cell");
		return nullptr;
	}

	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBuffer_Picked);
#endif
}
