#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CEnvironment : public CGameObject
{
public:
	enum EnvironmentID {NPC, ID_END };
public:
	typedef struct Envi_Desc
	{
		_float Scale;
		_float3 Rotation;
		_float RotationAngle;
		_float4 Pos;
	};
public:
	void Set_Pick(_bool picked) { m_bIsPicked = picked; }
	HRESULT Set_Update(Envi_Desc* _dec);
	void Set_Pos(_vector _pos);
	Envi_Desc* Get_Dec() { return &m_EnviDec; }
	_uint Get_NumAnim();
	void Set_AnimIndex();
	_int* Get_AnimIndex() { return &m_Animindex; }

protected:
	CEnvironment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnvironment(const CEnvironment& rhs);
	virtual ~CEnvironment() = default;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

	_bool IsInterSected();
	_bool IsPlayerApproach(_float fDist);
protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	_bool							m_bIsPicked = false;
	Envi_Desc						m_EnviDec = {};
	_int							m_Animindex = 0;
	CGameObject* m_pPlayer = nullptr;
	CCollider* m_pColliderCom = { nullptr };
protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ShaderResources() = 0;
public:
	virtual void Free() override;
};

END