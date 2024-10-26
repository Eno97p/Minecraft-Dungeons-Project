#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEmerald_UI :  public CInvenUI
{

private:
	CEmerald_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEmerald_UI(const CEmerald_UI& rhs);
	virtual ~CEmerald_UI() = default;

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
	_int* m_pNumemerald = nullptr;
public:
	static CEmerald_UI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END