#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CInven_ItemINFO :  public CInvenUI
{
public:
	typedef struct ITEMINFOINFO : public INVENDESC
	{
		_int iItemIndex;
		const wchar_t* ItemName;
		const wchar_t* TextureTag;
		ITEMRARITY		eRarity;
	};
private:
	CInven_ItemINFO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInven_ItemINFO(const CInven_ItemINFO& rhs);
	virtual ~CInven_ItemINFO() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	HRESULT Add_Components(const wchar_t* TextureTag);
	HRESULT Bind_ShaderResources();
private:
	void Set_Info_Font();

private:
	const wchar_t* m_ItemName;
	const wchar_t* m_ItemInfo;
	const wchar_t* m_ItemInfo2;
	_int m_Itemindex = 0;
	_float4 NameColor = {};
public:
	static CInven_ItemINFO* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END