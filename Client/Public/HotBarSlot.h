#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CHotBarSlot :  public CInvenUI
{
private:
	CHotBarSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHotBarSlot(const CHotBarSlot& rhs);
	virtual ~CHotBarSlot() = default;

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
	static CHotBarSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END