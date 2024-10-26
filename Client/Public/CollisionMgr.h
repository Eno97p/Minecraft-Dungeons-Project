#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"
BEGIN(Engine)
class CCollider;
class CGameObject;
END

BEGIN(Client)
class CollisionMgr : public CBase
{
	DECLARE_SINGLETON(CollisionMgr)

public:
	CollisionMgr();
	virtual ~CollisionMgr() = default;
public:
	void Add_Monster();
	void Add_Player_Weapon(CGameObject* weapon, _float WeaponDamage, CPlayer::WEAPON_STATE WeaponType);
public:
	void Tick(_float fTimeDelta);
	void Clear();
private:
	_uint WeaponParticle();
private:
	vector<CGameObject*> m_pCollvec;
	CGameObject* PlayerWeapon = nullptr;
	_float PlayerDamage = 0.f;
	CPlayer::WEAPON_STATE Type;



public:
	virtual void Free() override;
};
END