#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"
#include "Player.h"

BEGIN(Client)
class CHPBar :  public CInvenUI
{
public:
	typedef struct HPBARDESC : public INVENDESC
	{
		CPlayer::PLAYERSTATUS* pPlayerStat;
	};

private:
	CHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHPBar(const CHPBar& rhs);
	virtual ~CHPBar() = default;

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
	_float	m_fRatio = 0.f;
	_float	m_PreHP = 0.f;
	CPlayer::PLAYERSTATUS* m_PlayerStatus = nullptr;
	_float	m_Diff = 0.f;
	_float	m_pDiffRatio = 0.f;
	_int	textureChange = 0.f;
	_float	Offset = 0.1f;
public:
	static CHPBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END