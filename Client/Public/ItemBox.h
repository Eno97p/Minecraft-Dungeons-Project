#pragma once

#include "Client_Defines.h"
#include "Environment.h"
BEGIN(Engine)
class CCollider;
END
BEGIN(Client)

class CItemBox final : public CEnvironment
{
private:
	CItemBox(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItemBox(const CItemBox& rhs);
	virtual ~CItemBox() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
private:
	HRESULT Create_Item(_uint _iIndex);
	HRESULT Create_Emerald(_uint iNumEmerald);

private:

	_bool	m_IsOpen[2] = { false,false };
	static _uint iCount;

public:
	static CItemBox* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END