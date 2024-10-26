#pragma once
#include "ShopUI.h"
#include "Client_Defines.h"

BEGIN(Client)
class CEpicGround :  public CShopUI
{
public:
	typedef struct EpicGroundDesc : public GAMEOBJECT_DESC
	{
		const _float4x4* WorldTrans;
		_bool* Isdead;
	};
private:
	CEpicGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEpicGround(const CEpicGround& rhs);
	virtual ~CEpicGround() = default;

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
	_bool* m_IsDead = nullptr;

	_float m_fFrame = 0.f;
	_float m_MaxFrame = 11.f;
	_float4 vPos = {};
public:
	static CEpicGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END