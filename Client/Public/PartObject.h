#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTransform;
END

/* 파츠들(무기, 바디)의 부모가 되기위한 클래스이다. */

BEGIN(Client)

class CPartObject abstract : public CGameObject
{
public:
	typedef struct PARTOBJ_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4*		pParentMatrix;
		const _uint*			pState;
		const _uint*			pWeaponState;
		const _bool*			pRandomValue;
	};

protected:
	CPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPartObject(const CPartObject& rhs);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_vector Get_Part_Pos()
	{
		return XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);
	}

protected:
	_float4x4				m_WorldMatrix;
	const _float4x4*		m_pParentMatrix = { nullptr };
	const _uint*			m_pState = { nullptr };
	const _uint*			m_pWeapon = { nullptr };
	const _bool*			m_pRandomValue = { nullptr };

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END