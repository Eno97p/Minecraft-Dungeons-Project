#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CFKey :  public CShopUI
{
public:
	typedef struct FONTKEYDESC : public GAMEOBJECT_DESC
	{
		_float4 vPos;
	};
private:
	CFKey(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFKey(const CFKey& rhs);
	virtual ~CFKey() = default;

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
	_float4 m_vPos;


public:
	static CFKey* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END