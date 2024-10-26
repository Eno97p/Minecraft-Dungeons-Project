#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopFrame :  public CShopUI
{
private:
	CShopFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopFrame(const CShopFrame& rhs);
	virtual ~CShopFrame() = default;

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

public:
	static CShopFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END