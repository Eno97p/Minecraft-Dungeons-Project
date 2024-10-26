#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CItemRarity :  public CInvenUI
{
public:
	typedef struct RARITYDESC : public INVENDESC
	{
		ITEMRARITY		eRarity;
	};
private:
	CItemRarity(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemRarity(const CItemRarity& rhs);
	virtual ~CItemRarity() = default;

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
	static CItemRarity* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END