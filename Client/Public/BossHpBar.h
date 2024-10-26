#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CBossHpBar :  public CUI
{
public:
	typedef struct CBOSSBARDESC : public UIDESC
	{
		_float* MaxHp;
		_float* CurrentHp;
		_float2 FontPos;
		const wchar_t* FontString;
	};


private:
	CBossHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossHpBar(const CBossHpBar& rhs);
	virtual ~CBossHpBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	
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
	_float2 m_FontPos = {};
	const wchar_t* FontString = nullptr;
public:
	static CBossHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END