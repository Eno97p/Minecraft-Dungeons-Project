#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CSkeletonHorse final : public CMonster
{
	enum SKELETONHORSMAN_STATE{ STATE_IDLE,STATE_TURNING, STATE_RUN, STATE_WALK, STATE_NOEVENT, STATE_ATTACK ,STATE_SPAWN, STATE_END };
private:
	CSkeletonHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeletonHorse(const CSkeletonHorse& rhs);
	virtual ~CSkeletonHorse() = default;

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
	void Shoot_Arrow();
	void SetAnimIndex(_float fTimeDelta);

private:
	virtual NodeStates Patroll(_float fTimeDelta) override;
	NodeStates Turning(_float fTimeDelta);
	NodeStates Moving(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates NoEvents(_float fTimeDelta);
	NodeStates Spawn(_float fTimeDelta);
	NodeStates DEAD(_float fTimeDelta);

private:
	void StepSound();
	void DeathSound();
	virtual void HurtSound() override;
private:
	CGameObject* m_Bow = nullptr;
	CAttackBox* m_AttackBox = nullptr;
	SKELETONHORSMAN_STATE m_pCurState = STATE_IDLE;
private:
	_bool IsAttacking = false;
	_float m_fAttackCooltime = 1.f;

public:
	static CSkeletonHorse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END