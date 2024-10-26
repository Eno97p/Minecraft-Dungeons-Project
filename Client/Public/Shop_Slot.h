#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopSlot :  public CShopUI
{
private:
	CShopSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopSlot(const CShopSlot& rhs);
	virtual ~CShopSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool m_bSlotPicked = false;

public:
	static CShopSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END