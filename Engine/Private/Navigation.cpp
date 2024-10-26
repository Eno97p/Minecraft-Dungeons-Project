#include "..\Public\Navigation.h"

#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Cell.h"
#include "Transform.h"
_float4x4	CNavigation::m_WorldMatrix{};

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation & rhs)
	: CComponent{ rhs }
	, m_Cells{rhs.m_Cells}	
	, m_iCurrentCellIndex{rhs.m_iCurrentCellIndex }
	, m_iMouseCellIndex{rhs.m_iMouseCellIndex }
	, m_bPickmode{rhs.m_bPickmode }
	, m_bPickedPos{rhs.m_bPickedPos }
	, m_fHeight{rhs.m_fHeight }

#ifdef _DEBUG
	, m_pShader{rhs.m_pShader}
#endif
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif
}

HRESULT CNavigation::Initialize_Prototype(string FilePath)
{
	//ifstream inFile(strNavigationDataFile, std::ios::binary);
	//if (!inFile.good())		//경로 안에 파일이 없으면
	//{
	//	MSG_BOX("No File In Path");	//isFile 을 false하고 리턴
	//	return E_FAIL;
	//}
	//if (!inFile.is_open()) {
	//	// 이진 파일 열기 실패
	//	MSG_BOX("Failed To Open File");
	//	return E_FAIL;
	//}
	//else
	//{
	//	_float3		vPoint[CCell::POINT_END] = {};
	//	_uint Numvertex = 0.f;
	//	inFile.read((char*)&Numvertex, sizeof(_uint));
	//	for (int i = 0; i < Numvertex; ++i)
	//	{

	//		inFile.read((char*)&vPoint, sizeof(_float3) * 3);
	//		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, m_Cells.size());
	//		if (nullptr == pCell)
	//			return E_FAIL;

	//		m_Cells.emplace_back(pCell);
	//	}
	//	inFile.close();
	//}
	if (FAILED(Load_Navigation(FilePath)))
		return E_FAIL;


#pragma region HFILE
	/*_ulong		dwByte = {};
	HANDLE		hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoint[CCell::POINT_END] = {};

	while (true)
	{
		ReadFile(hFile, vPoint, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vPoint, m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);
	}

	CloseHandle(hFile);*/
#pragma endregion HFILE
	
	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize_Prototype()
{

#ifdef _DEBUG	
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return S_OK;

	m_iCurrentCellIndex = static_cast<NAVIGATION_DESC*>(pArg)->iCurrentCellIndex;
	if (m_iCurrentCellIndex == 0 && static_cast<NAVIGATION_DESC*>(pArg)->vPos != nullptr)
	{
		_float4 vPos = *static_cast<NAVIGATION_DESC*>(pArg)->vPos;
		_int			iNeighborIndex = { -1 };
		for (int i = 0; i < m_Cells.size(); ++i)
		{
			if (m_Cells[i]->isIn(XMLoadFloat4(&vPos), &iNeighborIndex))
			{
				m_iCurrentCellIndex = i;
				break;
			}
		}
	}

	return S_OK;
}

void CNavigation::ReCalculate_Neighbor(_vector vfPos)
{

	_float4 vPos;
	XMStoreFloat4(&vPos, vfPos);
	_int			iNeighborIndex = { -1 };
	for (int i = 0; i < m_Cells.size(); ++i)
	{
		if (m_Cells[i]->isIn(XMLoadFloat4(&vPos), &iNeighborIndex))
		{
			m_iCurrentCellIndex = i;
			break;
		}
	}
	
}

HRESULT CNavigation::Create_Cell(_float3* Points)
{
	_float3 vPoint[CCell::POINT_END] = {};
	memcpy(vPoint, Points, sizeof(_float3) * 3);
	if (m_Cells.size() == 0)
	{
		SortCell(vPoint);
		CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, 0);
		if (nullptr == pCell)
			return E_FAIL;

		m_Cells.emplace_back(pCell);

		return S_OK;
	}


	Compare_Cell(vPoint);
	SortCell(vPoint);
	CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoint, m_Cells.size());
	if (nullptr == pCell)
		return E_FAIL;

	m_Cells.emplace_back(pCell);

	Organize_Cells();
	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

	return S_OK;
}

void CNavigation::Delete_Last_Cell()
{
	if (m_Cells.size() == 0)
		return;
	Safe_Release(m_Cells.back());
	m_Cells.pop_back();
	SetUp_Neighbors();
}

void CNavigation::Delete_CurrentCell()
{
	if (m_Cells.size() == 0)
		return;

	if (m_iMouseCellIndex >= m_Cells.size())
		return;

	Safe_Release(m_Cells[m_iMouseCellIndex]);
	m_Cells.erase(m_Cells.begin() + m_iMouseCellIndex);
	SetUp_Neighbors();

}

void CNavigation::SortCell(_float3* Points)
{
	_vector vUP = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR ab = XMLoadFloat3(&Points[1]) - XMLoadFloat3(&Points[0]);
	XMVECTOR bc = XMLoadFloat3(&Points[2]) - XMLoadFloat3(&Points[1]);
	
	// ab와 bc의 크로스 제품 계산
	XMVECTOR cross = XMVector4Normalize(XMVector3Cross(ab, bc));

	float dotY = XMVectorGetX(XMVector4Dot(cross, vUP));

	if (dotY < 0)
	{
		std::swap(Points[1], Points[2]);
	}

  }

void CNavigation::Organize_Cells()
{
	_int iIndex = 0;
	for (auto& cell : m_Cells)
	{
		_float3* pPoints = cell->Get_All_Points();

		if (Compare_Points(pPoints[0], pPoints[1]) || Compare_Points(pPoints[1], pPoints[2]) || Compare_Points(pPoints[0], pPoints[2]))
		{
			Safe_Release(cell);
			m_Cells.erase(m_Cells.begin() + iIndex);
		}
		++iIndex;
	}

}

_int* CNavigation::Get_NeighborIndex()
{
	if (m_Cells.size() < 1)
		return nullptr;

	if (m_iMouseCellIndex == -1)
		return nullptr;

	if (m_Cells.size() <= m_iMouseCellIndex)
		return nullptr;

	return m_Cells[m_iMouseCellIndex]->Get_NeighborIndex();
	
}



void CNavigation::Compare_Cell(_float3* Points)
{
	if (m_Cells.size() < 1)
		return;

	/*_float3* LastCellpos = Get_Last_Cells_Position();*/

	for (auto& iter : m_Cells)
	{
		_float3* cellpoints = iter->Get_All_Points();
		
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (LengthCalculate(XMLoadFloat3(&cellpoints[j]), XMLoadFloat3(&Points[i])))
					Points[i] = cellpoints[j];
			}
		}
	}

}



_bool CNavigation::LengthCalculate(_vector Src, _vector Dst)
{
	if (XMVector3Length(Src - Dst).m128_f32[0] < 0.5f)
		return true;
	else
		return false;
}

_float3* CNavigation::Get_Last_Cells_Position()
{
	return m_Cells.back()->Get_All_Points();
}

HRESULT CNavigation::Save_Navigation(string filePath)
{
	if (m_Cells.size() < 1)
	{
		MSG_BOX("저장할 셀이 부족합니다.");
		return E_FAIL;
	}
	
	/*string finalPath = "../../Client/Bin/BinaryFile/Navigation.dat";*/
	ofstream file(filePath, ios::out | ios::binary);

	if (!file.is_open())
	{
		file.close();
		return E_FAIL;
	}
	else
	{
		_uint Size = m_Cells.size();
		file.write((char*)&Size, sizeof(_uint));

		for (auto& cell : m_Cells)
		{
			cell->Save_Cells(file);
		}
		MSG_BOX("Success");
		file.close();
	}
}

HRESULT CNavigation::Load_Navigation(string filePath)
{

	/*string finalPath = "../../Client/Bin/BinaryFile/Navigation.dat";*/


	ifstream inFile(filePath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		_uint CellSize = 0;
		inFile.read((char*)&CellSize, sizeof(_uint));
		_int index = 0;
		for (int i = 0; i < CellSize; ++i)
		{
			CCell* Cell = CCell::LoadCreate(m_pDevice, m_pContext, inFile);
			SortCell(Cell->Get_All_Points());
			Cell->Setup_Index(index);
			m_Cells.push_back(Cell);
			++index;
		}
		if (FAILED(SetUp_Neighbors()))
			return E_FAIL;

		inFile.close();
	}

	return S_OK;
}

void CNavigation::Teleport_Random_Cell_Approach(CTransform* pTransform, _float fDist)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vLocalPos;

	XMStoreFloat3(&vLocalPos, XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix))));


	vector<_vector> Positions;
	for (auto& iter : m_Cells)
	{
		_vector cellpos = iter->Get_Center_Point();
		if (XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&vLocalPos), cellpos))) < fDist)
		{
			Positions.emplace_back(cellpos);
		}
	}

	_int numsize = Positions.size();
	_int RandomNumber = RandomInt(0, numsize-1);
	_vector ResultPosition = XMVector3TransformCoord(Positions[RandomNumber] , XMLoadFloat4x4(&m_WorldMatrix));


	pTransform->Set_State(CTransform::STATE_POSITION, ResultPosition);
	ReCalculate_Neighbor(ResultPosition);

}

_float4 CNavigation::Get_Random_Cell_Point(_float fDist)
{
	_vector vCellPos = m_Cells[m_iCurrentCellIndex]->Get_Center_Point();

	vector<_vector> Positions;
	for (auto& iter : m_Cells)
	{
		_vector cellpos = iter->Get_Center_Point();
		if (XMVectorGetX(XMVector3Length(XMVectorSubtract(vCellPos, cellpos))) < fDist)
		{
			Positions.emplace_back(cellpos);
		}
	}
	_int numsize = Positions.size();
	_int RandomNumber = RandomInt(0, numsize -1);
	_vector ResultPosition = XMVector3TransformCoord(Positions[RandomNumber], XMLoadFloat4x4(&m_WorldMatrix));

	_float4 ResultValue;
	XMStoreFloat4(&ResultValue, ResultPosition);
	//ResultValue.w = 1.f;
	return ResultValue;
}



void CNavigation::Update(CTransform* pTransform)
{
	m_WorldMatrix = *pTransform->Get_WorldFloat4x4();
	_matrix worldmat = XMLoadFloat4x4(&m_WorldMatrix);
	_float3 vScale = pTransform->Get_Scaled();

	for (int i = 0; i < 3; ++i)
	{
		worldmat.r[i] = XMVector4Normalize(worldmat.r[i]);
	}
	XMStoreFloat4x4(&m_WorldMatrix, worldmat);

	if(m_bPickmode)
		IsMouseOn();

}

_bool CNavigation::isMove(_fvector vPosition)
{
	_vector		vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));
	
	_int			iNeighborIndex = { -1 };

	/* 셀 안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex))
	{
		return true;
	}
	/* 셀 밖으로 움직였다. */
	else
	{
		if (-1 != iNeighborIndex)
		{
			_int iCount = 0;
			while (true)
			{
				if (iCount > 100)
				{
					ReCalculate_Neighbor(vLocalPos);
					return true;
				}
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex))
				{
					m_iCurrentCellIndex = iNeighborIndex;
					return true;
				}			
				iCount++;
			}
			
		}
		else
			return false;
	}
}

_bool CNavigation::isMove(_fvector vPosition, _float4* LineDir)
{
	_vector      vLocalPos = XMVector3TransformCoord(vPosition, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_int         iNeighborIndex = { -1 };

	
	/* 셀 안에서 움직였다. */
	if (true == m_Cells[m_iCurrentCellIndex]->isIn(vLocalPos, &iNeighborIndex, LineDir))
		return true;

	/* 셀 밖으로 움직였다. */
	else
	{
		if (-1 != iNeighborIndex)
		{
			_int iCount = 0;
			while (true)
			{
				if (iCount > 100)
				{
					ReCalculate_Neighbor(vLocalPos);
					return true;
				}
				if (-1 == iNeighborIndex)
					return false;

				if (true == m_Cells[iNeighborIndex]->isIn(vLocalPos, &iNeighborIndex, LineDir))
				{
					m_iCurrentCellIndex = iNeighborIndex;
					return true;
				}
				iCount++;
			}

		}
		else
			return false;
	}
}



void CNavigation::Compute_Height(CTransform* pTransform)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vLocalPos;
	
	XMStoreFloat3(&vLocalPos, XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix))));

	vLocalPos.y = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos);

	_vector Result = XMVector3TransformCoord(XMLoadFloat3(&vLocalPos), XMLoadFloat4x4(&m_WorldMatrix));
	pTransform->Set_State(CTransform::STATE_POSITION, Result);
	m_fHeight = XMVectorGetY(Result);
}

void CNavigation::Compute_Height(CTransform* pTransform, _float y)
{
	_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_float3 vLocalPos;

	XMStoreFloat3(&vLocalPos, XMVector3TransformCoord(vPos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix))));

	vLocalPos.y = m_Cells[m_iCurrentCellIndex]->Compute_Height(vLocalPos) + y;

	_vector Result = XMVector3TransformCoord(XMLoadFloat3(&vLocalPos), XMLoadFloat4x4(&m_WorldMatrix));
	pTransform->Set_State(CTransform::STATE_POSITION, Result);
	m_fHeight = XMVectorGetY(Result);


}

void CNavigation::IsMouseOn()
{
	using namespace TriangleTests;

	_float3 Raypos, RayDir;
	m_pGameInstance->Compute_LocalRayInfo(&RayDir, &Raypos, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)));

	_vector vRaypos = XMLoadFloat3(&Raypos);
	_vector vRaydir = XMLoadFloat3(&RayDir);

	_float fDist;
	_uint index = 0;
	for (auto& cell : m_Cells)
	{
		_float3* pPoints = cell->Get_All_Points();

		if (TriangleTests::Intersects(vRaypos, XMVector3Normalize(vRaydir),
			XMLoadFloat3(&pPoints[0]),
			XMLoadFloat3(&pPoints[1]),
			XMLoadFloat3(&pPoints[2]),
			fDist))
		{
			cell->Set_Picked(true);
			m_iMouseCellIndex = index;
			_vector pickPos = vRaypos + (XMVector3Normalize(vRaydir) * fDist);
			XMStoreFloat4(&m_bPickedPos, pickPos);
		}
		else
		{
			cell->Set_Picked(false);
		}
		++index;
	}

}

_uint CNavigation::Get_MouseIndex()
{
	if (m_Cells.size() < 1)
		return 0;
	else
		return m_iMouseCellIndex;
	
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
	if (m_Cells.size() == 0)
		return S_OK;

	m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

	m_pShader->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW));
	m_pShader->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ));

	if (-1 == m_iCurrentCellIndex)
	{
		_float4 colorG = { 0.f, 1.f, 0.f, 1.f };

		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &colorG, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		for (auto& pCell : m_Cells)
			pCell->Render();

		return S_OK;
	}

	else
	{
		_float4x4	Matrix = m_WorldMatrix;

		Matrix._42 += 0.1f;

		m_pShader->Bind_Matrix("g_WorldMatrix", &Matrix);

		_float4 colorR = { 1.f, 0.0f, 0.f, 1.f };
		if (FAILED(m_pShader->Bind_RawValue("g_vColor", &colorR, sizeof(_float4))))
			return E_FAIL;

		m_pShader->Begin(0);

		m_Cells[m_iCurrentCellIndex]->Render();

		return S_OK;
	}

}

#endif

HRESULT CNavigation::SetUp_Neighbors()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
				pSourCell->SetUp_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->SetUp_Neighbor(CCell::LINE_BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->SetUp_Neighbor(CCell::LINE_CA, pDestCell);
		}
	}
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, string FilePath)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(FilePath)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		return nullptr;
	}

	return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNavigation* pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavigation");
		return nullptr;
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNavigation");
		return nullptr;
	}

	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
#endif
}
