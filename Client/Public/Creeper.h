#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Client)

class CCreeper final : public CMonster
{
	enum CREEPERSTATE{ STATE_IDLE, STATE_RUN, STATE_WALK, STATE_NOEVENT,STATE_ATTACK, STATE_STUN,STATE_HIT, STATE_END };
private:
	CCreeper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreeper(const CCreeper& rhs);
	virtual ~CCreeper() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT SetStatus() override;

public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

private:
	void SetAnimIndex(_float fTimeDelta);

	virtual HRESULT Add_Nodes() override;
	virtual NodeStates Patroll(_float fTimeDelta) override;

	NodeStates MoveToPlayer(_float fTimeDelta);
	NodeStates Attack(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Stun(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);

public:
	static CCreeper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CREEPERSTATE m_pCurState = { STATE_IDLE };
	_bool IsAttacking = false;
	float c_blinkInterval = 0.50f;
	float c_blinkTimer = 0.0f;
	float c_redLightDuration = 0.2f;
	float c_fAccTime = 6.f;


};

END