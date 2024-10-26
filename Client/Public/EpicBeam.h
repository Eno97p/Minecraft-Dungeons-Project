#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CTransform;
END

BEGIN(Client)
class CEpicBeam :  public CShopUI
{
public:
	typedef struct EpicDesc : public GAMEOBJECT_DESC
	{
		const _float4x4* WorldTrans;
	};
private:
	CEpicBeam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEpicBeam(const CEpicBeam& rhs);
	virtual ~CEpicBeam() = default;

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

	const _float4x4* parentMat = nullptr;

	_float m_fFrame = 0.f;
	_float m_MaxFrame = 10.f;
	_float4 vPos = {};
public:
	static CEpicBeam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END