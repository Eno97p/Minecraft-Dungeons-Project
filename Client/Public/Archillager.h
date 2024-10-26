#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CArchillager final : public CMonster
{
	enum ARCHILLAGERSTATE {STATE_APPEAR, STATE_IDLE, STATE_BASIC_ATTACK, STATE_BASIC_JUMP_ATTACK,
		STATE_SUMMON_CIRCLE, STATE_SUMMON_LINE, STATE_SUMMON_SINGLE, STATE_FLYING, 
		STATE_LANDING, STATE_RUN,
	STATE_DEFEAT, STATE_END };

private:
	CArchillager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArchillager(const CArchillager& rhs);
	virtual ~CArchillager() = default;

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
	void Teleport();
	void Summon_Monster(_int Loop);
	void Summon_HeartOfEnder();
	void Create_Core();
	void Create_Core_Circle();
	void Create_Following_Core();

	NodeStates Basic_Attack(_float fTimeDelta);
	NodeStates RandomPattern(_float fTimeDelta);
	NodeStates Summon_Line(_float fTimeDelta);
	NodeStates Summon_Circle(_float fTimeDelta);
	NodeStates Summon_Single(_float fTimeDelta);
	NodeStates TeleportToRandom(_float fTimeDelta);
	NodeStates Landing(_float fTimeDelta);
	NodeStates Defeat(_float fTimeDelta);

	NodeStates Move(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);


	void RandomSound();
	virtual void HurtSound() override;
	
private:
	class CFreeCamera* m_Camera = nullptr;
	class CAttackBox* m_Orb = nullptr;
	_float m_fParticleInterval = 0.1f;
	ARCHILLAGERSTATE m_pCurState = STATE_APPEAR;
public:
	static CArchillager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END