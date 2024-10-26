#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CArrow : public CGameObject
{
public:
	typedef struct ArrowDesc : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float4 vDir;
		_bool bTypeFrendly; //true 면 아군, false 면 적군
	};

private:
	CArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArrow(const CArrow& rhs);
	virtual ~CArrow() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	void ShootSound();
	void HitSound();

	void Collision();

private:
	_float4			m_vDir;
	_float4			m_vPos;
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	_float4x4				m_WorldMatrix;
	_bool					m_bType;
	_float			m_fLifeTime = 6.f;

public:
	static CArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END