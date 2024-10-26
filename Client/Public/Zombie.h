#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CZombie final : public CMonster
{
	enum ZOMBIESTATE { STATE_IDLE, STATE_RUN, STATE_WALK, STATE_NOEVENT, STATE_ATTACK, STATE_STUN, STATE_HIT, STATE_END };
private:
	CZombie(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CZombie(const CZombie& rhs);
	virtual ~CZombie() = default;

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
	HRESULT AddPartsObjects();
private:
	void SetAnimIndex(_float fTimeDelta);
	virtual HRESULT Add_Nodes() override;
	virtual NodeStates Patroll(_float fTimeDelta) override;

private: //nodes
	NodeStates MoveToPlayer(_float fTimeDelta);
	NodeStates Attack(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Stun(_float fTimeDelta);
	NodeStates Hold(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);

	void AttackSound();
	void Voice();
	void HitSound();
	virtual void HurtSound() override;
	void DeathSound();
public:
	static CZombie* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	vector<class CAttackBox*>		m_PartObjects;
	ZOMBIESTATE m_pCurState = { STATE_IDLE };
	_bool IsAttacking = false;
};

END