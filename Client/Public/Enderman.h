#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Client)

class CEnderman final : public CMonster
{
	enum ENDERMANSTATE { STATE_IDLE, STATE_RUN, STATE_WALK, STATE_NOEVENT,STATE_ATTACK, STATE_STUN,STATE_HIT, STATE_END };
private:
	CEnderman(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnderman(const CEnderman& rhs);
	virtual ~CEnderman() = default;

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

	NodeStates MoveToPlayer();
	NodeStates Attack();
	NodeStates IDLE();
	NodeStates Hit();
	NodeStates Stun();
public:
	static CEnderman* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	ENDERMANSTATE m_pCurState = { STATE_IDLE };
	_bool IsAttacking = false;

	
};

END