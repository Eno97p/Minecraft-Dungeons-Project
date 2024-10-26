#pragma once

#include "Client_Defines.h"
#include "Armor.h"

BEGIN(Client)

class Cham_Head final : public CArmor
{
private:
	Cham_Head(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Cham_Head(const Cham_Head& rhs);
	virtual ~Cham_Head() = default;

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
	static Cham_Head* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END