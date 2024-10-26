#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CTransform;
END

BEGIN(Client)
class CCore_Bomb : public CGameObject
{
public:
	typedef struct CoreBombDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		CGameObject* pTarget;
	};

private:
	CCore_Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCore_Bomb(const CCore_Bomb& rhs);
	virtual ~CCore_Bomb() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	void Collision();

private:
	_float4			m_vPos;
	_float3			m_vRandomAxis;

	CTransform* m_pTargetTransform  = nullptr;
	CGameObject* m_pTarger = nullptr;
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	_float4x4				m_WorldMatrix;
	_float			m_fLifeTime = 0.f;

public:
	static CCore_Bomb* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END