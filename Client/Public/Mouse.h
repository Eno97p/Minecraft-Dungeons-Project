#pragma once
#include "UI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CMouse :  public CUI
{
private:
	CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMouse(const CMouse& rhs);
	virtual ~CMouse() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_float4* GetMousePos() { return &m_vMousePos; }
	_float4							m_vMousePos;
	
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMouse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END