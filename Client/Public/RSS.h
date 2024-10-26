#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
END

BEGIN(Client)
class RSS : public CGameObject
{
public:
	typedef struct RSSDESC : public CGameObject::GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float4 vDir;
		_float4 vColor;
		_uint ParticleNum;
	};

private:
	RSS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	RSS(const RSS& rhs);
	virtual ~RSS() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_float4			m_vDir;
	_float4			m_vPos;
	_float4			m_vPoriginPos;

	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CNavigation* m_pNavigationCom = nullptr;
	_float4x4				m_WorldMatrix;

	class CPlayer* m_pPlayer = nullptr;

	_float4 m_vColor;
	_uint	m_iParticleNumber = 0;
	_float RandomSpeed = 0.f;
	_float fLifeTime = 2.f;


	_float m_fRatio = 0.f;
	_float m_fDuration = 0.3f;

	_bool IsBlur = false;

public:
	static RSS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END