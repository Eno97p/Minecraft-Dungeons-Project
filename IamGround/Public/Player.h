#pragma once

#include "Tool_Define.h"
#include "Monster.h"

BEGIN(TOOL)

class CPlayer final : public CGameObject
{
public:
	typedef struct PLAYERDEC
	{
		_float Scale;
		_float3 Rotation;
		_float RotationAngle;
		_float4 Pos;
	};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	PLAYERDEC Get_Dec() { return m_PlayerDec; }
	void Set_Pos(_vector _pos);

public:
	virtual HRESULT Add_Components();
	virtual HRESULT Bind_ShaderResources();

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool							m_bIsPicked = false;
	PLAYERDEC						m_PlayerDec = {};
	_int							m_Animindex = 0;
public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END