#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CInven_ItemIcon :  public CInvenUI
{
public:
	typedef struct ITEMINFO : public INVENDESC
	{
		const wchar_t* ItemName;
		const wchar_t* TextureTag;
		_int ItemIndex;
	};
private:
	CInven_ItemIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInven_ItemIcon(const CInven_ItemIcon& rhs);
	virtual ~CInven_ItemIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	EQUIPSLOTUSAGE Get_Usage() { return m_eType; }
	_int Get_ItemNumber() { return m_ItemNum; }
	virtual void Resize(_float4 PositionValue) override;

public:
	HRESULT Add_Components(const wchar_t* TextureTag);
	HRESULT Bind_ShaderResources();

private:
	void Set_Rarity();
private:
	EQUIPSLOTUSAGE m_eType;
	_int			m_ItemNum = 0;
	CGameObject* m_ItemInfo = nullptr;
	CGameObject* m_Rarity = nullptr;
	ITEMRARITY		m_eRarity = RARITY_END;
public:
	static CInven_ItemIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END