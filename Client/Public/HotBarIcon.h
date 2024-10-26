#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CHotBarIcon :  public CInvenUI
{
public:
	typedef struct HOTBARICON : public INVENDESC
	{
		const wchar_t* TextureTag;
		const wchar_t* Font;
		_int*		Value;
		_float2		FontPosition;
		_float2		Font2Position;
	};
private:
	CHotBarIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHotBarIcon(const CHotBarIcon& rhs);
	virtual ~CHotBarIcon() = default;

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
	const wchar_t* m_Font;
	_int*		m_Value = nullptr;
	_float2		FontPosition = {};
	_float2		FontPosition2 = {};
public:
	static CHotBarIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END