#pragma once
#include "InvenUI.h"
#include "Client_Defines.h"
#include "Player.h"
BEGIN(Engine)
class CModel;
END

BEGIN(Client)
class CDummyPlayer :  public CInvenUI
{
private:
	CDummyPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDummyPlayer(const CDummyPlayer& rhs);
	virtual ~CDummyPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void SetPlayer(CPlayer* pPlayer);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_PartObjects();
private:
	CModel* m_pModelCom = { nullptr };
	vector<class CGameObject*>		m_Armors[CPlayer::ARMOR_END];
	vector<class CGameObject*>		m_Weapons;
	CPlayer* m_pPlayer = nullptr;
	_uint* m_iCurrentWeapon;
	_uint* m_iCurrentArmor; 
public:
	static CDummyPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END