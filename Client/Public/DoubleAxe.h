#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CDoubleAxe final : public CWeapon
{

private:
	CDoubleAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoubleAxe(const CDoubleAxe& rhs);
	virtual ~CDoubleAxe() = default;

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

public:
	static CDoubleAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END