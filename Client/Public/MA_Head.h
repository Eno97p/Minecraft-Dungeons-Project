#pragma once

#include "Client_Defines.h"
#include "Armor.h"

BEGIN(Client)

class CMA_Head final : public CArmor
{
private:
	CMA_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMA_Head(const CMA_Head& rhs);
	virtual ~CMA_Head() = default;

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
	static CMA_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END