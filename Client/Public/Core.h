#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CCore : public CGameObject
{
public:
	typedef struct CoreDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float4 vDir;
		_float4 vColor;
		_uint ParticleNum;
		_float fSpeed = 1.f;
		_float fSize = 0.2f;
	};

private:
	CCore(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCore(const CCore& rhs);
	virtual ~CCore() = default;

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
	_float4			m_vDir;
	_float4			m_vPos;
	_float3			m_vRandomAxis;
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CGameObject* m_pTarget = nullptr;
	_float4x4				m_WorldMatrix;
	_float			m_fLifeTime = 0.f;
	_float4			m_vColor = {1.f,1.f,1.f,1.f};
	_uint			m_ParticleNum = 0;
	_float			m_fSpeed = 0.f;



public:
	static CCore* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END