#pragma once

#include "Client_Defines.h"
#include "Armor.h"

BEGIN(Client)

class Cham_L_Arm final : public CArmor
{
private:
	Cham_L_Arm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Cham_L_Arm(const Cham_L_Arm& rhs);
	virtual ~Cham_L_Arm() = default;

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
	static Cham_L_Arm* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END