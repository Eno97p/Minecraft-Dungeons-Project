#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CGolem final : public CMonster
{
	enum GOLEMSTATE { STATE_IDLE,STATE_BASIC_ATTACK,STATE_CHARGE_ATTACK, STATE_STRONG_ATTACK,STATE_SUPER_ATTACK, STATE_SWIPE_ATTACK,
		STATE_WALK,STATE_CHASE, STATE_NOEVENT,STATE_STUN, STATE_TURNING, STATE_END };
	enum GOLEMPARTS { LEFT_HAND, RIGHT_HAND, PARTS_END };
private:
	CGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGolem(const CGolem& rhs);
	virtual ~CGolem() = default;

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
	HRESULT AddPartsObjects();
	virtual HRESULT Bind_ShaderResources() override;
private:
	virtual HRESULT Add_Nodes() override;
	virtual NodeStates Patroll(_float fTimeDelta) override;

	void SetAnimIndex(_float fTimeDelta);

private:
	vector<class CAttackBox*>		m_PartObjects;
	GOLEMSTATE m_pCurState = STATE_IDLE;

private: //nodes
	NodeStates MoveToPlayer(_float fTimeDelta);
	NodeStates SwipeAttack(_float fTimeDelta);
	NodeStates ChargeAttack(_float fTimeDelta);
	NodeStates StrongAttack(_float fTimeDelta);

	NodeStates Turning(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Stun(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);

	void SwipeSound();
	void StrongAttackSound();
	void WalkSound();
	void DeadSound();
	void GrowlSound();
	virtual void HurtSound() override;

private:
	_bool bSwipeAttack = false;
	_float ChargeTime = 0.2f;
	_bool StrongAttacked = false;
public:
	static CGolem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END