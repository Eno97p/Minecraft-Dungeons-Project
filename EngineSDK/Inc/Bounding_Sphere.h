#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere final : public CBounding
{
public:
	typedef struct SPHERE_DESC : public CBounding::BOUNDING_DESC
	{
		_float		fRadius;
	};
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	virtual void* Get_Bounding() override {
		return m_pSphere;
	}

public:
	HRESULT Initialize(void* pArg);
	virtual void Tick(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

	_bool Intersect(CCollider::TYPE eType, CBounding * pBounding);
	virtual _bool IntersectRay(_vector vRayOrigin, _vector vRayDirection, _float fDistance) override;
	virtual _float3 Get_Center();
private:
	BoundingSphere*				m_pOriginalSphere = { nullptr };
	BoundingSphere*				m_pSphere = { nullptr };

public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg);
	virtual void Free() override;
};

END