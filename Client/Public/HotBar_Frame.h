#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CHotBarFrame :  public CInvenUI
{
private:
	CHotBarFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHotBarFrame(const CHotBarFrame& rhs);
	virtual ~CHotBarFrame() = default;

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
	static CHotBarFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END