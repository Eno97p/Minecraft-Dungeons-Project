#pragma once

#include "Client_Defines.h"
#include "Weapon.h"

BEGIN(Client)

class CSpear final : public CWeapon
{

private:
	CSpear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpear(const CSpear& rhs);
	virtual ~CSpear() = default;

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
	static CSpear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END