#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

BEGIN(Client)
class CCircle :	 public CBlendObject
{
public:
	typedef struct CIRCLEDESC : public GAMEOBJECT_DESC
	{
		_float LifeTimeMax;
		_float fRadius;
		_float4 vPos;
		_uint	ParticleNum;
	};

private:
	CCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCircle(const CCircle& rhs);
	virtual ~CCircle() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CCollider*		 m_pColliderCom = { nullptr };	
	class CPlayer* m_pPlayer = { nullptr };

	_float			m_fRatio = 0;
	_float			m_fLifetime = 0;
	_float			m_fLifetimeMax = 0;
	_float			m_fRadius = 0;
	_float3			m_f4Pos = {};
	_uint			m_ParticleNum;
	void MakeParticle();
	void ExplosionSound();
	void SmallExplosion();

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;



};

END