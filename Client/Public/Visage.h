#pragma once

#include "Client_Defines.h"
#include "Monster.h"


BEGIN(Client)

class CVisage final : public CMonster
{

private:
	CVisage(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVisage(const CVisage& rhs);
	virtual ~CVisage() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
private:
	virtual HRESULT Add_Nodes() override;
	virtual NodeStates Patroll(_float fTimeDelta) override;
	void Generate_Lazer();
	void BeamAttackSound();
	void BeamLoop(_float fTimeDelta);

	_bool m_Out = false;
	_bool m_Bool = false;
	_float fBeamInterval = 0.f;

public:
	static CVisage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	// CMonster을(를) 통해 상속됨
	HRESULT SetStatus() override;
};

END