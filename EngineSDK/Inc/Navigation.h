#pragma once

#include "Component.h"

/* 1. 객체가 움직일 수 있는 길을 삼각형의 집합으로 표현하자. */
/* 2. 셀들의 정보를 기반으로해서 움직일 수 있냐? 없냐? 검사를 수행한다. */
BEGIN(Engine)
class CTransform;
END
BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct NAVIGATION_DESC
	{
		_int		iCurrentCellIndex = { -1 };
		_float4*	vPos = nullptr;
	};
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(string FilePath);
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	void ReCalculate_Neighbor(_vector vfPos);
	HRESULT Create_Cell(_float3* Points);
	int Get_Cell_Size() { return m_Cells.size(); }
	void Delete_Last_Cell();
	void Delete_CurrentCell();
	void SortCell(_float3* Points);
	void Organize_Cells();
	_int* Get_NeighborIndex();
	_float Get_Height() { return m_fHeight; }
	
	_float3* Get_Last_Cells_Position();
	HRESULT Save_Navigation(string filePath);
	HRESULT Load_Navigation(string filePath);

	void Teleport_Random_Cell_Approach(CTransform* pTransform, _float fDist);
	_float4 Get_Random_Cell_Point(_float fDist);
public:
	void Update(CTransform* pTransform);
	_bool isMove(_fvector vPosition);
	_bool isMove(_fvector vPosition, _float4* LineDir);
	void Compute_Height(CTransform* pTransform);
	void Compute_Height(CTransform* pTransform , _float y);
	void IsMouseOn();
	_uint Get_MouseIndex();
	void Set_PickMode(_bool value) { m_bPickmode = value; }
	_bool Get_PickMode() {
		return m_bPickmode;
	}
	_float4* Get_PickedPos() {
			return &m_bPickedPos;
	}

	_int Get_CurrentCellIndex()
	{
		return m_iCurrentCellIndex;
	}
#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif
	
private:
	vector<class CCell*>		m_Cells;
	static _float4x4			m_WorldMatrix;
	_int						m_iCurrentCellIndex = { -1 };
	_int						m_iMouseCellIndex = { -1 };
	_bool						m_bPickmode = false;
	_float4				m_bPickedPos = {};
	_float				m_fHeight = 0.f;

#ifdef _DEBUG
	class CShader*				m_pShader = { nullptr };	
#endif

private:
	HRESULT SetUp_Neighbors();
	_bool LengthCalculate(_vector Src, _vector Dst);
	void Compare_Cell(_float3* Points);
	_bool Compare_Points(_float3& vSrc, _float3& vDst) {
		return vSrc.x == vDst.x && vSrc.y == vDst.y && vSrc.z == vDst.z;
	}

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, string FilePath);
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free();

};

END