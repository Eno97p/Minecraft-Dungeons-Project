#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CBehaviorTree;
class CNavigation;
class CCollider;
class CModel;
END

/* 플레이어를 구성하는 객체들을 들고 있는 객체이다. */

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	typedef struct PLAYERDEC
	{
		_float Scale;
		_float3 Rotation;
		_float RotationAngle;
		_float4 Pos;
	};

	typedef struct PLAYERSTATUS
	{
		_float fCurHP;
		_float fMaxHP;
		_float fSpeed;
		_float fATK;
		_float fDFS;
		_int   iNumPotion;
		_int   iNumArrows;
	};


public:
	enum PART { PART_BODY, PART_WEAPON,PART_BOW, DBAXE_LEFT, DBAXE_RIGHT, PART_END };

	enum STATE { STATE_IDLE,
		STATE_RUN, STATE_ATTACK, STATE_SHOOT,
		STATE_DASH, STATE_HIT, STATE_DEAD,STATE_REVIVE, STATE_END };

	enum WEAPON_STATE {	E_SWORD, E_BOW, E_DOUBLEAXE, E_SPEAR, E_AXE, E_KATANA, E_HAMMER, E_Claymore, WEAPON_END };
	enum ARMOR_STATE { NONARMOR, MERCENARY,CHAMPION, ARMOR_END};
private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;	

	void Set_Damage(_float fDamage);

	void Change_Equipment(_int ItemNumber);

	void Create_Arrow();
	void Create_Core();
	void Summon_Monster(_float fTimeDelta);	//visage 소환
	void Summon_Visage();
	void hitSound();
	_vector Get_Pos();

	_bool Intersect(CCollider* pDstCollidercom);
private:
	NodeStates Dead(_float fTimeDelta);
	NodeStates Revive(_float fTimeDelta);
	NodeStates Hit(_float fTimeDelta);
	NodeStates BigHit(_float fTimeDelta);
	NodeStates Dash(_float fTimeDelta);
	NodeStates Move(_float fTimeDelta);
	NodeStates ShootArrow(_float fTimeDelta);
	NodeStates ATTACK(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);

private:
	HRESULT Add_Components();	
	HRESULT Add_PartObjects();
	HRESULT Add_Nodes();
	void Set_Status_Struct();
	void Eat_Potion();
public:
	void Swift();
public:
	_uint* Get_CurWeapon() { return &m_iCurrentWeapon; }
	_uint* Get_CurAmo() { return &m_iCurrentArmor; }
	PLAYERSTATUS* Get_Status() { return &m_Status; }

public:
	void Set_Hit(_bool _isHit, _bool _isHitStrong);
	void HammerAttack();
	_float* Get_CameraDegree() { return m_CameraDegree; }

private:
	vector<class CGameObject*>		m_PartObjects;
	vector<class CGameObject*>		m_Armors[ARMOR_END];
	vector<class CGameObject*>		m_Weapons;
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_Body = nullptr;
	_uint							m_iCurrentState = { 0 };
	_uint							m_iCurrentWeapon = { 0 };
	_uint							m_iPreWeapon = { 0 };
	_uint							m_iCurrentArmor = { 0 };
	CBehaviorTree*					 m_ComBehavior = nullptr;
	CNavigation*					m_pNavigationCom = { nullptr };
	CNavigation*					m_pMapNavigation = nullptr;

	_bool			HammerAttacked = false;

	_bool			isHit = false;
	_float			HitReset = 0.1f;	//맞았을때 셰이더 바뀌는거 초기화용



	_bool			isHitStrong = false;
	_bool			randomvalue = false;


	_bool			isLanded[2] = { false,false };
	_float4			m_vPos;

	_vector			m_PrePos;		//마우스를 찍은 시점에서 기억해놓을 위치
	_vector			m_vDestPos;		//마우스를 찍은 위치
	_vector			m_MoveVector;	//마우스를 찍은 시점에서의 위치에서 m_vDestPos를 뺀 방향벡터
	_float*			m_CameraDegree;

	_bool*			m_IsInventoryOn = nullptr;
	_bool*			m_IsShopOn = nullptr;
	_bool			m_IsSwift = false;


	_float fLandtime;


	HRESULT Create_Item();
	HRESULT Create_Emerald();
	void Create_Weapon_All();
	void Generate_Lazer();

	void SwordSwing();
	void DoubleaxeSwing();
	void KatanaSwing();
	void SpearSwing();
	void ClaymoreSwing();
	void AxeSwing();
	void HammerSwing();

	void EatPotionSound();

	void RandomMonsterSummon();
	void SkeletonHorseSummon();
	void MakeFireWorks();

	void DashSound();

	PLAYERSTATUS	m_Status = {};
	_float			m_HitTimeRatio = 0.f;
		
	_float fVisageAcctime = 0.f;
	const _float finterval = 0.7f;


	_bool SecondDoorOpen = false;


	_float FireWorks = 0.f;
	_bool FireworksStart = false;

	


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END