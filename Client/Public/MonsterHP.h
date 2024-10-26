#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CMonsterHpBar :  public CUI
{
public:
	typedef struct CMonsterHPDesc : public UIDESC
	{
		_float* MaxHp;
		_float* CurrentHp;
		const _float4x4*	ParentMat;
	};

private:
	CMonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterHpBar(const CMonsterHpBar& rhs);
	virtual ~CMonsterHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_YLength(_float fLength)
	{
		m_YLength = fLength;
	}
	
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
private:
	_float* m_CurHp = nullptr;
	_float* m_MaxHp = nullptr;
	_float m_PreHp = 0.f;
	_float m_Diff = 0.f;
	_float  m_pRatio = 0.f;
	_float	m_pDiffRatio = 0.f;
	_float	m_YLength = 1.5f;
	const _float4x4* m_ParentMatrix = nullptr;

public:
	static CMonsterHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END