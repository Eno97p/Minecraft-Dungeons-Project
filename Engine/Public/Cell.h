#pragma once

#include "Base.h"

/* 1. 네비게이션을 구성하는 하나의 삼가형. */
/* 2. 삼각형을 구성하는 세점의 정보를 보관한다. */
/* 3. 세 변을 구성하여 객체의 결과위치가 안에 있는지? 없는지? 조사.  */
BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}
	_vector Get_Center_Point();
	_float3* Get_All_Points() { return m_vPoints; }
	HRESULT Save_Cells(ofstream& os);
	HRESULT Load_Cells(ifstream& is);
	_float Compute_Height(_float3 vLocalpos);
	void Setup_Index(_int _i) {
		m_iIndex = _i;
	}

public:
	HRESULT Initialize(const _float3* pPoints, _int iIndex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool isIn(_fvector vPoint, _int* pNeighborIndex);
	_bool isIn(_fvector vPoint, _int* pNeighborIndex, _float4* LineDir);
	void SetUp_Neighbor(LINE eNeighbor, CCell* pCell) {
		if (pCell == nullptr)
		{
			m_iNeighborIndices[eNeighbor] = -1;
		}
		else
			m_iNeighborIndices[eNeighbor] = pCell->m_iIndex;
	}
	void Set_Picked(_bool pick) {
		isPicked = pick;
	}
	_int* Get_NeighborIndex() { return m_iNeighborIndices; }

#ifdef _DEBUG
public:
	HRESULT Render();

#endif

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

private:
	_float3				m_vPoints[POINT_END] = {};
	_int				m_iIndex = { 0 };
	_int				m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
	_bool				isPicked = false;

#ifdef _DEBUG

private:
	class CVIBuffer_Cell*		m_pVIBuffer = { nullptr };
	class CVIBuffer_Cell_Pick*	m_pVIBuffer_Picked = { nullptr };
#endif
	
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	static CCell* LoadCreate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& is);
	virtual void Free() override;
};

END