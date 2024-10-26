#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CInven_MainSlot :  public CInvenUI
{
private:
	CInven_MainSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInven_MainSlot(const CInven_MainSlot& rhs);
	virtual ~CInven_MainSlot() = default;

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
	static CInven_MainSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END