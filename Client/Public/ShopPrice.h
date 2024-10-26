#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopPrice :  public CShopUI
{
public:
	typedef struct PRICEDESC : public SHOPDESC
	{
		_int iPrice;
	};
private:
	CShopPrice(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopPrice(const CShopPrice& rhs);
	virtual ~CShopPrice() = default;

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
	_int m_iPrice = 0;

public:
	static CShopPrice* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END