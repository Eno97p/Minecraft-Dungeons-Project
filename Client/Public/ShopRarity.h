#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopRarity :  public CShopUI
{
public:
	typedef struct RARITYDESC : public SHOPDESC
	{
		ITEMRARITY		eRarity;
	};
private:
	CShopRarity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopRarity(const CShopRarity& rhs);
	virtual ~CShopRarity() = default;

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
	_int RarityIndex = 0;

public:
	static CShopRarity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END