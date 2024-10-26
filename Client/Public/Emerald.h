#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
END

BEGIN(Client)
class CEmerald :	public CGameObject
{
public:
	typedef struct EMERALDDESC : public GAMEOBJECT_DESC
	{
		_float4 vPos;
		_float3 vDir;
	};

private:
	CEmerald(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEmerald(const CEmerald& rhs);
	virtual ~CEmerald() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CNavigation*	 m_pNavigationCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_bool IsPlayerApproach(_float fDist);
	_vector Get_PlayerPos();

private:
	class CPlayer* pPlayer;
	EMERALDDESC Description = {};
	_float m_fRotationPower = 0.f;
public:
	static CEmerald* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END