#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)

class CAttackBox final : public CPartObject
{
public:
	typedef struct ATTACKBOX_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;
		CCollider::TYPE				eType;
		_float3		vCenter;
		_float		vRadius;	//for Sphere
		_float3		vExtents;	//for AABB
	};

private:
	CAttackBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAttackBox(const CAttackBox& rhs);
	virtual ~CAttackBox() = default;

public:
	_bool InterSect(CCollider* SrcCol);	//내 콜라이더가 충돌 당한거로
	_bool InterSected(CCollider* SrcCol); //상대방 콜라이더를 충돌당한거로 
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_float4x4* Get_ATB_Mat();
	_vector Get_ATB_Pos();

private:
	virtual HRESULT Add_Components(CCollider::TYPE ColType);

private:
	CCollider*						 m_pColliderCom = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

	_float3		m_vCenter = {};
	_float		m_vRadius = {};
	_float3		m_vExtents = {};
	CCollider::TYPE m_eType = CCollider::TYPE::TYPE_END;
public:
	static CAttackBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END