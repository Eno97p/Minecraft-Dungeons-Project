#pragma once

#include "Client_Defines.h"
#include "Armor.h"

BEGIN(Client)

class CMA_L_L final : public CArmor
{
private:
	CMA_L_L(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMA_L_L(const CMA_L_L& rhs);
	virtual ~CMA_L_L() = default;

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
	static CMA_L_L* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END