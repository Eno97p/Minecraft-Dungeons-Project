#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CVindicator final : public CMonster
{
	enum VINDICATORSTATE { STATE_IDLE,STATE_RUN, STATE_WALK, STATE_NOEVENT, STATE_ATTACK, STATE_STUN, STATE_END };
private:
	CVindicator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVindicator(const CVindicator& rhs);
	virtual ~CVindicator() = default;

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
	CGameObject* m_Axe = nullptr;
	VINDICATORSTATE m_pCurState = STATE_IDLE;

private: //nodes
	NodeStates MoveToPlayer(_float fTimeDelta);
	NodeStates Attack(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates Stun(_float fTimeDelta);
	NodeStates Hold(_float fTimeDelta);
	NodeStates Dead(_float fTimeDelta);

private:
	void IDLESound();
	virtual void HurtSound() override;
	void DeadSound();
	
private:
	_bool IsAttacking = false;
public:
	static CVindicator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END