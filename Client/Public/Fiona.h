#pragma once

#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CFiona final : public CMonster
{
private:
	CFiona(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFiona(const CFiona& rhs);
	virtual ~CFiona() = default;

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
	virtual HRESULT SetStatus() override;
public:
	static CFiona* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END