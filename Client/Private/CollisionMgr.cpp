#include "stdafx.h"
#include "CollisionMgr.h"
#include "GameInstance.h"
#include "Monster.h"
IMPLEMENT_SINGLETON(CollisionMgr)

CollisionMgr::CollisionMgr()
{
}

void CollisionMgr::Add_Monster()
{
	list<class CGameObject*> monster = GetGI->Get_Layer_Objects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	for (auto& iter : monster)
	{
		if (iter->Get_Dead() == true)
			continue;
		else
		{
			m_pCollvec.emplace_back(iter);
			Safe_AddRef(iter);
		}
	}
}

void CollisionMgr::Add_Player_Weapon(CGameObject* weapon, _float WeaponDamage, CPlayer::WEAPON_STATE WeaponType)
{
	PlayerWeapon = weapon;
	PlayerDamage = WeaponDamage;
	Type = WeaponType;
}

void CollisionMgr::Tick(_float fTimeDelta)
{
	if (PlayerWeapon == nullptr)
		return;
	
	Add_Monster();
	for (auto& monster : m_pCollvec)
	{
		if (monster->InterSected(PlayerWeapon))
		{
			static_cast<CMonster*>(monster)->Set_Damage(PlayerDamage, WeaponParticle());
		}
	}
	Clear();
}

void CollisionMgr::Clear()
{
	for (auto& iter : m_pCollvec)
		Safe_Release(iter);

	m_pCollvec.clear();
	PlayerWeapon = nullptr;
	PlayerDamage = 0.f;
}

_uint CollisionMgr::WeaponParticle()
{
	_uint Particleindex = 0;
	switch (Type)
	{
	case CPlayer::E_SWORD:
		Particleindex = 0;
		break;
	case CPlayer::E_DOUBLEAXE:
		Particleindex = 1;
		break;
	case CPlayer::E_SPEAR:
		Particleindex = 2;
		break;
	case CPlayer::E_AXE:
		Particleindex = 3;
		break;
	case CPlayer::E_KATANA:
		Particleindex = 4;
		break;
	case CPlayer::E_Claymore:
		Particleindex = 6;
		break;
	default:
		break;

	}

	return Particleindex;
}



void CollisionMgr::Free()
{
}
