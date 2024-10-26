#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CRedStoneBoss final : public CMonster
{
	enum REDSTONESTATE {
		STATE_IDLE, STATE_AWAKE ,STATE_BASIC_ATTACK,STATE_LAVA_ATTACK,STATE_SLAMATTACK, STATE_STRONG_ATTACK,
		STATE_SPLIT_ATTACK,
		STATE_CHASE , STATE_STUN ,STATE_DEFEAT, STATE_END };
	enum REDSTONEPARTS { LEFT_HAND, RIGHT_HAND, HEAD, PARTS_END };
private:
	CRedStoneBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRedStoneBoss(const CRedStoneBoss& rhs);
	virtual ~CRedStoneBoss() = default;

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

private:
	void Generate_RSS();
	void Generate_StrongAttack();
	void Generate_Missile();

	void SetAnimIndex(_float fTimeDelta);
private:
	vector<class CAttackBox*>		m_PartObjects;
	REDSTONESTATE m_pCurState = STATE_AWAKE;

private: //nodes
	NodeStates LavaAttack(_float fTimeDelta);
	NodeStates SlamAttack(_float fTimeDelta);
	NodeStates StrongAttack(_float fTimeDelta);
	NodeStates BasicAttack(_float fTimeDelta);
	NodeStates SplitAttack(_float fTimeDelta);
	NodeStates MoveToPlayer(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Defeat(_float fTimeDelta);
	NodeStates Stun(_float fTimeDelta);

	virtual void HurtSound() override;
	void ExplosionSound();

	
private:
	class CFreeCamera* m_Camera = nullptr;
	_bool m_StrongAttack = false;
public:
	static CRedStoneBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END