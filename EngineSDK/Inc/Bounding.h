#pragma once

#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{

public:
	typedef struct BOUNDING_DESC
	{
		_float3		vCenter;
	};

protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual void* Get_Bounding() = 0;

public:
	virtual void Tick(_fmatrix WorldMatrix) = 0;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBounding) = 0;
	virtual _bool IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance) = 0;
	virtual _float3 Get_Center() = 0;


protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	_bool						m_isColl = { false };

public:	
	virtual void Free() override;
};

END