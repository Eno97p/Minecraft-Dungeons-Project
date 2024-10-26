#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CShopNomoney :  public CShopUI
{
public:
	typedef struct WARNINGDESC : public SHOPDESC
	{
		_bool bType;
	};


private:
	CShopNomoney(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShopNomoney(const CShopNomoney& rhs);
	virtual ~CShopNomoney() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Sound_Yes();
	void Sound_No();


public:
	HRESULT Add_Components();
private:
	_float m_fRatio = 0.f;
	_float m_fLifeTime = 2.f;
	_bool m_bType = false; //false면 돈 부족 true면 구입완료

public:
	static CShopNomoney* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END