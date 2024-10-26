#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	typedef struct AABB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3		vExtents;
	};
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pAABB;
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	virtual _bool Intersect(CCollider::TYPE eType, CBounding* pBounding) override;
	virtual _bool IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance) override;
	virtual _float3 Get_Center();
private:
	BoundingBox*				m_pOriginalAABB = { nullptr };
	BoundingBox*				m_pAABB = { nullptr };

private:
	_bool Intersect(BoundingBox* pTargetAABB);


public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END