#include "stdafx.h"
#include "..\Public\Player.h"

#include "GameInstance.h"
#include "PartObject.h"
#include "Weapon.h"
#include "Body_Player.h"
#include "Layer.h"
#include "Monster.h"
#include "Arrow.h"
#include "Inventory.h"
#include "Item.h"

#include "Circle.h"		//디버그
#include "Emerald.h"	//디버그
#include "Armor.h"
#include "CollisionMgr.h"
#include "Core.h"		//디버그
#include "Lazer.h"		//디버그
#include "FreeCamera.h"// 디버그
#include "Door.h"

#include "ParticleManager.h"


_float fSwingVolume = 0.2f;
_float fEffectVolume = 0.3f;
CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject{ rhs }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	CGameObject::GAMEOBJECT_DESC	GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	_vector vPos = XMLoadFloat4(&((PLAYERDEC*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_vPos = ((PLAYERDEC*)pArg)->Pos;

	Set_Unique_ID("Player");

	m_Status.fSpeed = 1.f;
	m_Status.fCurHP = 100.f;
	m_Status.iNumPotion = 2;
	m_Status.iNumArrows = 20;
	Set_Status_Struct();

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	if (FAILED(Add_Nodes()))
		return E_FAIL;
	
	Set_Unique_ID("Player");
	/* 플레이어의 Transform이란 녀석은 파츠가 될 바디와 웨폰의 부모 행렬정보를 가지는 컴포넌트가 될거다. */	

	m_vDestPos = XMVectorZero();
	m_pTransformCom->Set_Power(2.f);
	_float a = 0;
	m_CameraDegree = &a;

	CInventory::GetInstance()->Initialize(this);
	m_IsInventoryOn = CInventory::GetInstance()->Get_InvenOpen();
	m_IsShopOn = CInventory::GetInstance()->Get_ShopOpen();



	return S_OK;
}

void CPlayer::Priority_Tick(_float fTimeDelta)
{

	if (*m_IsInventoryOn == true || *m_IsShopOn == true)
		return;

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Priority_Tick(fTimeDelta);

	m_Weapons[m_iCurrentWeapon]->Priority_Tick(fTimeDelta);
	if (m_iCurrentArmor != 0)
	{
		for (auto& pPartArmor : m_Armors[m_iCurrentArmor])
			pPartArmor->Priority_Tick(fTimeDelta);
	}
	//m_CameraDegree
	*m_CameraDegree = XMVectorGetX(XMVector3Length(m_pGameInstance->Get_CamPosition() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (*m_IsInventoryOn == true || *m_IsShopOn == true)
		return;

	m_ComBehavior->Update(fTimeDelta);
	Eat_Potion();
#pragma region DEBUGINGTEST


	if (m_iCurrentState == STATE_DEAD && m_pGameInstance->isKeyDown('R'))
	{
		m_iCurrentState = STATE_REVIVE;
		m_Body->StopAnimation(false);
		m_ComBehavior->Set_Activate(true);
		m_Status.fCurHP = m_Status.fMaxHP;
	}

	if (!SecondDoorOpen)
	{
		if (m_pNavigationCom->Get_CurrentCellIndex() == 1614
			|| m_pNavigationCom->Get_CurrentCellIndex() == 1616)
		{
			CFreeCamera* pcamera = static_cast<CFreeCamera*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0));
			pcamera->Set_ChangeTarget_Point(XMVectorLerp(XMVectorSet(-198.93f, 45.51f, 30.62f, 1.f),
				XMVectorSet(-192.22f, 45.51f, 30.60f, 1.f), 0.5f));
			pcamera->Set_ShakeCamera(0.1f, 5.f, true);
			m_pGameInstance->PlaySound_Z(_T("EarthQuake.mp3"), SOUND_EFFECT, 0.5f);
			m_pGameInstance->PlaySound_Z(_T("DoorOpen.ogg"), SOUND_EFFECT, 0.3f);
			static_cast<CDoor*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Door"), 0))->Set_Door_Open();
			SecondDoorOpen = true;
		}
	}

	if (m_pNavigationCom->Get_CurrentCellIndex() == 1584 && !isLanded[0])
	{
		fLandtime = 0.5f;
		isLanded[0] = true;
	}

	static _int iLandCount = 0;
	if (isLanded[0] == true && iLandCount < 50)
	{
		fLandtime -= fTimeDelta;
		if (fLandtime < 0.f)
		{
			RandomMonsterSummon();
			fLandtime = 0.5f;
			iLandCount++;
		}

	}

	if (m_pNavigationCom->Get_CurrentCellIndex() == 1416 && !isLanded[1])
	{
		SkeletonHorseSummon();
		isLanded[1] = true;
	}


	if (m_pNavigationCom->Get_CurrentCellIndex() == 1713 && !FireworksStart)
	{
		CGameObject* Fiona = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), "Fiona");
		m_pGameInstance->StopAll();
		m_pGameInstance->PlaySound_Z(_T("bgm_env_missionFinished-003.OGG"), SOUND_EFFECT, 0.3f);
		CParticleManager::GetInstance()->Create_Particle(69, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		FireworksStart = true;
	}
	if (FireworksStart)
	{
		static _float FireInterval = 1.5f;
		static _bool Naration = false;
		FireWorks += fTimeDelta;
		CFreeCamera* pcamera = static_cast<CFreeCamera*>(m_pGameInstance->Get_Object(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), 0));
		pcamera->Distance_Up(fTimeDelta);

		if (FireWorks > 3.f && !Naration)
		{
			m_pGameInstance->PlaySound_Z(_T("vo_cin_soswOutro-001.OGG"), SOUND_EFFECT, 0.3f);
			Naration = true;
		}
		if (FireWorks > 5.f)
		{
			//m_pGameInstance->PlayBGM(_T(""))
			FireInterval -= fTimeDelta;
			if (FireInterval < 0.f)
			{
				FireInterval = 1.5f;
				MakeFireWorks();
			}
		}

	}



	if (m_pGameInstance->isKeyDown(0x39)) 	//보스방 순간이동 9번키
	{
		_vector vBossPos = XMVectorSet(-196.f, 45.51, 19.54, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBossPos);
		m_pNavigationCom->ReCalculate_Neighbor(vBossPos);
	}
	if (m_pGameInstance->isKeyDown(0x38)) 	//중간보스방 순간이동 8번키
	{
		_vector vDestPos = XMVectorSet(26.57, 18.51, -138.12, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
		m_pNavigationCom->ReCalculate_Neighbor(vDestPos);
	}
	if (m_pGameInstance->isKeyDown(0x37)) 	//7번키  보스방 앞
	{
		_vector vDestPos = XMVectorSet(-152.06, 46.25, -56.13, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
		m_pNavigationCom->ReCalculate_Neighbor(vDestPos);
	}
	if (m_pGameInstance->isKeyDown(0x36)) 	//6번키  중간보스방 뒤
	{
		_vector vDestPos = XMVectorSet(-37.91, 30.50, -94.79, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDestPos);
		m_pNavigationCom->ReCalculate_Neighbor(vDestPos);
	}

	if (m_pGameInstance->isKeyDown(0x33)) 	//6번키  중간보스방 뒤
	{
		Summon_Visage();
	}


	

	if (m_pGameInstance->isKeyDown('M'))
		Create_Weapon_All();
	if (m_pGameInstance->isKeyDown('B'))
	{
		Create_Emerald();
	}

	if (m_pGameInstance->isKeyDown('K'))
	{
		CItem::ItemDesc itemdesc{};
		itemdesc.fData = 1;
		itemdesc.fRadius = 0.1f;
		itemdesc.RandomValue = 0;

		XMStoreFloat4(&itemdesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);

	}

	if (m_IsSwift)
	{
		static _float SwiftDelta = 25.f;
		SwiftDelta -= fTimeDelta;
		if (SwiftDelta < 0.f)
		{
			SwiftDelta = 25.f;
			m_IsSwift = false;
			m_Status.fSpeed = 1.f;
		}
	}

	if (m_HitTimeRatio > 0.0f)
	{
		m_HitTimeRatio -= fTimeDelta / 0.4f;
		if (m_HitTimeRatio < 0.0f) {
			m_HitTimeRatio = 0.0f;
		}
	}

#pragma endregion DEBUGINGTEST

	if (GetAsyncKeyState(VK_RBUTTON) && m_iCurrentState != STATE_SHOOT && m_iCurrentState != STATE_DASH)
	{
		if (m_Status.iNumArrows > 0)
		{
			m_iCurrentState = STATE_SHOOT;
			_float4* vLookat = m_pMapNavigation->Get_PickedPos();
			m_pTransformCom->LookAt_For_LandObject(XMLoadFloat4(vLookat));
			Create_Arrow();
		}
	}


	if (GetAsyncKeyState(VK_LBUTTON) && m_iCurrentState != STATE_DASH)
	{
		if (m_pMapNavigation == nullptr)
		{
			m_pMapNavigation = static_cast<CNavigation*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Map"), "Map")->Get_Component(TEXT("Com_Navigation")));
		}
		m_PrePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vDestPos = XMLoadFloat4(m_pMapNavigation->Get_PickedPos());
		m_pTransformCom->LookAt_For_LandObject(m_vDestPos);
		m_MoveVector = m_vDestPos - m_PrePos;
		m_iCurrentState = STATE_RUN;
	}

	if (GetAsyncKeyState('A') && m_iCurrentState != STATE_DASH)
	{
		m_iCurrentState = STATE_ATTACK;
		randomvalue = checkCondition();

		_float4* vLookat = m_pMapNavigation->Get_PickedPos();
		m_pTransformCom->LookAt_For_LandObject(XMLoadFloat4(vLookat));
	}


	for (auto& pPartObject : m_PartObjects)
		pPartObject->Tick(fTimeDelta);

	if (m_iCurrentArmor == CHAMPION)
	{
		CParticleManager::GetInstance()->Create_Particle(67, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		CParticleManager::GetInstance()->Create_Particle(67, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	}



	if (m_iCurrentArmor != 0)
	{
		for (auto& pPartArmor : m_Armors[m_iCurrentArmor])
			pPartArmor->Tick(fTimeDelta);
	}
	m_Weapons[m_iCurrentWeapon]->Tick(fTimeDelta);

	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
	
}



void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (*m_IsInventoryOn == true || *m_IsShopOn == true)
		return;

	for (auto& pPartObject : m_PartObjects)
		pPartObject->Late_Tick(fTimeDelta);

	if (m_iCurrentArmor != 0)
	{
		for (auto& pPartArmor : m_Armors[m_iCurrentArmor])
			pPartArmor->Late_Tick(fTimeDelta);
	}
	m_Weapons[m_iCurrentWeapon]->Late_Tick(fTimeDelta);

	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);

	if (m_pTransformCom->isJumping())
	{
		m_pTransformCom->Jump(fTimeDelta * m_Status.fSpeed, m_pNavigationCom);
		m_pTransformCom->Go_Backward(fTimeDelta, m_pNavigationCom);
	}
	else
	{
		m_pNavigationCom->Compute_Height(m_pTransformCom);
	}

#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent(m_pColliderCom);
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif
}

HRESULT CPlayer::Render()
{	
	


	return S_OK;
}

void CPlayer::Set_Damage(_float fDamage)
{
	_float damageReduction = m_Status.fDFS / 100.f;
	float finalDamage = fDamage * (1.0f - damageReduction);
	
	m_Status.fCurHP -= finalDamage;
	if (m_Status.fCurHP < 0.0f) {
		m_Status.fCurHP = 0.0f;
	}
}


void CPlayer::Change_Equipment(_int ItemNumber)
{
	switch (ItemNumber)
	{
	case 2:
		m_iCurrentWeapon = E_KATANA;
		break;
	case 3:
		m_iCurrentWeapon = E_HAMMER;
		break;
	case 4:
		m_iCurrentWeapon = E_Claymore;
		break;
	case 5:
		m_iCurrentWeapon = E_DOUBLEAXE;
		break;
	case 6:
		m_iCurrentWeapon = E_AXE;
		break;
	case 7:
		m_iCurrentWeapon = E_SPEAR;
		break;
	case 8:
		m_iCurrentArmor = MERCENARY;
		break;
	case 9:
		m_iCurrentArmor = CHAMPION;
		break;
	case 10:
		m_iCurrentWeapon = E_SWORD;
		break;
	}

	Set_Status_Struct();

}




void CPlayer::Create_Arrow()
{
	CArrow::ArrowDesc Arrowdec = {};
	Arrowdec.bTypeFrendly = true;
	XMStoreFloat4(&Arrowdec.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	XMStoreFloat4(&Arrowdec.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Arrowdec);
	m_Status.iNumArrows--;
}

void CPlayer::Create_Core()
{
	CCore::CoreDesc cordec{};
	
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&cordec.vPos, vPos);

	float angleIncrement = 360.0f / 8.0f; // 45도 간격
	for (int i = 0; i < 8; ++i) {
		float angle = i * angleIncrement;
		float radian = XMConvertToRadians(angle);
		float x = cosf(radian);
		float z = sinf(radian);

		cordec.vDir = _float4(x, 0.0f, z, 0.f);
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Core"), &cordec);
	}



}

void CPlayer::Summon_Monster(_float fTimeDelta)
{
	fVisageAcctime += fTimeDelta;
	if (fVisageAcctime > finterval)
	{
		Summon_Visage();
		fVisageAcctime = 0.f;
	}
	
}

void CPlayer::Summon_Visage()
{
	CMonster::MONSTER_DEC dec{};
	CGameObject* monster = nullptr;
	dec.Pos = m_pNavigationCom->Get_Random_Cell_Point(6.f);
	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Scale = 1.f;
	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Visage"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
}

void CPlayer::hitSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}

_vector CPlayer::Get_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}



_bool CPlayer::Intersect(CCollider* pDstCollidercom)
{
	return m_pColliderCom->Intersect(pDstCollidercom);
}

NodeStates CPlayer::Dead(_float fTimeDelta)
{
	if (m_Status.fCurHP <= 0.f && m_iCurrentState != STATE_DEAD)
	{
		m_Status.fCurHP = 0.f;
		m_iCurrentState = STATE_DEAD;
		return RUNNING;
	}
	else if (m_iCurrentState == STATE_DEAD)
	{
		if (m_Body->Get_AnimFinished())
		{
			//
			m_Body->StopAnimation(true);
			m_ComBehavior->Set_Activate(false);
			return SUCCESS;
		}
		else
			return RUNNING;
	}
	else
		return FAILURE;

}

NodeStates CPlayer::Revive(_float fTimeDelta)
{
	if (m_iCurrentState != STATE_REVIVE)
		return FAILURE;
	else
	{
		if (m_Body->Get_AnimFinished())
		{
			m_iCurrentState = STATE_IDLE;
			return SUCCESS;
		}
		else
			return RUNNING;
	}
}

NodeStates CPlayer::Hit(_float fTimeDelta)
{
	if (isHit == false)
		return FAILURE;
	else //isHit이 true면
	{
		HitReset -= fTimeDelta;
		if (HitReset <= 0.f)
		{
			isHit = false;
			HitReset = 0.1f;
		}
		return COOLING;
	}
}

NodeStates CPlayer::BigHit(_float fTimeDelta)	//쿨타임 좀 주자
{
	if (m_iCurrentState != STATE_HIT)
		return FAILURE;
	else
	{
		if (m_Body->Get_AnimFinished())
		{
			isHitStrong = false;
			return SUCCESS;
		}
		else
		{
			return RUNNING;
		}
	}

}


NodeStates CPlayer::Dash(_float fTimeDelta)
{

	if (GetKeyState(VK_SPACE) & 0x8000 && m_iCurrentState != STATE_DASH)
	{
		_float4* vLookat = m_pMapNavigation->Get_PickedPos();
		m_pTransformCom->LookAt_For_LandObject(XMLoadFloat4(vLookat));
		m_iCurrentState = STATE_DASH;
		DashSound();
		return RUNNING;
	}
	
	if (m_iCurrentState == STATE_DASH)
	{
		if (m_Body->Get_AnimFinished())
		{
			m_iCurrentState = STATE_IDLE;
			return SUCCESS;
		}
		else
		{
			if (m_Body->Get_Ratio_Betwin(0.2f, 0.8f))
			{
				CParticleManager::GetInstance()->Create_Particle(60, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
			}

			m_pTransformCom->Go_Straight_Faster(fTimeDelta * m_Status.fSpeed, m_pNavigationCom);
			return RUNNING;
		}
	}
	else
	{
		return FAILURE;
	}
}

NodeStates CPlayer::Move(_float fTimeDelta)
{

	if (m_iCurrentState == STATE_RUN)
	{
		_vector vCurDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_PrePos;
		/*XMVectorSetY(vCurDir, 0.f);*/
		if (XMVectorGetX(XMVector3Length(m_MoveVector)) < XMVectorGetX(XMVector3Length(vCurDir)))
		{
			return FAILURE;
		}
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta* m_Status.fSpeed, m_pNavigationCom);
			return SUCCESS;
		}

		if (HammerAttacked)
			HammerAttacked = false;

	}
	else
		return FAILURE;

}

NodeStates CPlayer::ShootArrow(_float fTimeDelta)
{

	if (m_iCurrentState == STATE_SHOOT)
	{
		if (m_Body->Get_AnimFinished())
		{
			m_iCurrentState = STATE_IDLE;
			return FAILURE;
		}
		else
		{
			m_Weapons[E_BOW]->Late_Tick(fTimeDelta);
			return RUNNING;
		}
	}
	else
		return FAILURE;

}

NodeStates CPlayer::IDLE(_float fTimeDelta)
{
	m_iCurrentState = STATE_IDLE;
	return SUCCESS;
}



NodeStates CPlayer::ATTACK(_float fTimeDelta)
{

	static _bool SoundPlayed = false;


	if (m_iCurrentState == STATE_ATTACK)
	{
		//_float a = m_Body->Get_Current_Ratio();
		//cout << a << endl;

		if (m_Body->Get_AnimFinished())
		{
			/*isAttack = false;*/
			m_iCurrentState = STATE_IDLE;
			HammerAttacked = false;
			return FAILURE;
		}
		else
		{
			switch (m_iCurrentWeapon)
			{
			case E_SWORD:
				if (m_Body->Get_Ratio_Betwin(0.07f, 0.09f) || m_Body->Get_Ratio_Betwin(0.30f, 0.33f) ||
					m_Body->Get_Ratio_Betwin(0.66f, 0.69f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK , E_SWORD);
				}
				break;
			case E_SPEAR:
				if (m_Body->Get_Ratio_Betwin(0.15f, 0.17f) || m_Body->Get_Ratio_Betwin(0.39f, 0.45f) ||
					m_Body->Get_Ratio_Betwin(0.79f, 0.86f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_SPEAR);
				}
				break;
			case E_KATANA:
				if (m_Body->Get_Ratio_Betwin(0.09f, 0.13f) || m_Body->Get_Ratio_Betwin(0.30f, 0.36f) ||
					m_Body->Get_Ratio_Betwin(0.70f, 0.79f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_KATANA);
				}
				break;
			case E_Claymore:
				if (m_Body->Get_Ratio_Betwin(0.10f, 0.17f) || m_Body->Get_Ratio_Betwin(0.50f, 0.58f) ||
					m_Body->Get_Ratio_Betwin(0.80f, 0.86f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_Claymore);
				}
				break;
			case E_AXE:
				if (m_Body->Get_Ratio_Betwin(0.08f, 0.11f) || m_Body->Get_Ratio_Betwin(0.35f, 0.38f) ||
					m_Body->Get_Ratio_Betwin(0.70, 0.77f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_AXE);
				}
				break;
			case E_HAMMER:
				if (!SoundPlayed)
				{
					if (m_Body->Get_Current_Ratio() < 0.1f)
					{
						HammerSwing();
						SoundPlayed = true;
					}
				}

				if (m_Body->Get_Current_Ratio() > 0.53f && !HammerAttacked)
				{
					_vector HammerPos = static_cast<CPartObject*>(m_Weapons[m_iCurrentWeapon])->Get_Part_Pos();
					CParticleManager::GetInstance()->Create_Particle(26, HammerPos, nullptr);
					CParticleManager::GetInstance()->Create_Particle(27, HammerPos, nullptr);
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_HAMMER);
					//HammerAttack();
					HammerAttacked = true;
				}
				break;
			case E_DOUBLEAXE:
				if (m_Body->Get_Ratio_Betwin(0.20f, 0.30f) || m_Body->Get_Ratio_Betwin(0.70f, 0.80f))
				{
					CollisionMgr::GetInstance()->Add_Player_Weapon(m_Weapons[m_iCurrentWeapon], m_Status.fATK, E_DOUBLEAXE);
				}
				break;
			}
			return RUNNING;
		}
	}
	else
	{
		HammerAttacked = false;
		return FAILURE;
	}

}



	

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		TEXT("Com_Behavior"), reinterpret_cast<CComponent**>(&m_ComBehavior))))
	{
		return E_FAIL;
	}

	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(0.3f, 0.7f, 0.3f);
	aabbdesc.vCenter = _float3(0.f, aabbdesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbdesc)))
		return E_FAIL;


	CNavigation::NAVIGATION_DESC	NavigationDesc{};
	NavigationDesc.iCurrentCellIndex = 0;
	NavigationDesc.vPos = &m_vPos;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;
	
	/*m_pNavigationCom->Load_Navigation("../Bin/BinaryFile/Navigation.dat");*/

	return S_OK;
}

HRESULT CPlayer::Add_PartObjects()
{
	/* 바디객체를 복제해온다. */
	CPartObject::PARTOBJ_DESC		BodyDesc{};
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	BodyDesc.fSpeedPerSec = 0.f;
	BodyDesc.fRotationPerSec = 0.f;
	BodyDesc.pState = &m_iCurrentState;
	BodyDesc.pWeaponState = &m_iCurrentWeapon;
	BodyDesc.pRandomValue = &randomvalue;
	CGameObject*		pBody = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Body_Player"), &BodyDesc);
	if (nullptr == pBody)
		return E_FAIL;
	m_PartObjects.emplace_back(pBody);
	m_Body = static_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	/* 무기객체를 복제해온다. */

	CGameObject* pWeapon = nullptr;

	m_Weapons.reserve(WEAPON_END);

	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	WeaponDesc.pState = &m_iCurrentState;

	CModel*		pModelCom = dynamic_cast<CModel*>(pBody->Get_Component(TEXT("Com_Model")));
	if (nullptr == pModelCom)
		return E_FAIL;	

 	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	//Add Sword
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Sword"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Bow
	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;


	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Bow"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add DoubleAxe
	WeaponDesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;


	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DoubleAxe"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Spear
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Spear"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Axe
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Axe"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Katana
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Katana"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Hammer
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hammer"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Claymore
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Claymore"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	CGameObject* pArmor = nullptr;

	CArmor::ARMORDESC armordesc{};
	armordesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Body_armor");

	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_Body"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("J_Head");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_Head"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("L_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_L_Arm"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	/*armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("L_Leg_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_L_Leg"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("R_Leg_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_R_Leg"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);*/

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("R_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_R_Arm"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	/* ==============champions armor =============*/

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("Body_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_Body"), &armordesc);
	m_Armors[CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("J_Head");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_Head"), &armordesc);
	m_Armors[CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("L_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_L_Arm"), &armordesc);
	m_Armors[CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("R_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_R_Arm"), &armordesc);
	m_Armors[CHAMPION].emplace_back(pArmor);

	return S_OK;
}

HRESULT CPlayer::Add_Nodes()
{
	m_ComBehavior->Generate_Root(TEXT("Root"), CBehaviorTree::Sequence);
	m_ComBehavior->Add_Node(TEXT("Root"), TEXT("Top_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Hit_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Move_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("Attack_Selector"), CBehaviorTree::Selector);
	m_ComBehavior->Add_Node(TEXT("Top_Selector"), TEXT("IDLE"), CBehaviorTree::Action, bind(&CPlayer::IDLE, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("Hit_Selector"), TEXT("DEAD"), CBehaviorTree::Action, bind(&CPlayer::Dead, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Hit_Selector"), TEXT("REVIVE"), CBehaviorTree::Action, bind(&CPlayer::Revive, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Hit_Selector"), TEXT("HIT"), CBehaviorTree::Action, bind(&CPlayer::Hit, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Hit_Selector"), TEXT("BIGHIT"), CBehaviorTree::Action, bind(&CPlayer::BigHit, this, std::placeholders::_1), CoolDown, 0.5f);

	m_ComBehavior->Add_Node(TEXT("Move_Selector"), TEXT("DASH"), CBehaviorTree::Action, bind(&CPlayer::Dash, this, std::placeholders::_1), CoolDown, 1.5f);
	m_ComBehavior->Add_Node(TEXT("Move_Selector"), TEXT("MoveTo"), CBehaviorTree::Action, bind(&CPlayer::Move, this, std::placeholders::_1));

	m_ComBehavior->Add_Node(TEXT("Attack_Selector"), TEXT("Shoot"), CBehaviorTree::Action, bind(&CPlayer::ShootArrow, this, std::placeholders::_1));
	m_ComBehavior->Add_Node(TEXT("Attack_Selector"), TEXT("Attack"), CBehaviorTree::Action, bind(&CPlayer::ATTACK, this, std::placeholders::_1));


	return S_OK;
}

void CPlayer::Set_Status_Struct()
{
	switch (m_iCurrentWeapon)
	{
	case E_SWORD:
		m_Status.fATK = 10.f;
		break;
	case E_DOUBLEAXE:
		m_Status.fATK = 20.f;
		break;
	case E_SPEAR:
		m_Status.fATK = 40.f;
		break;
	case E_AXE:
		m_Status.fATK = 25.f;
		break;
	case E_KATANA:
		m_Status.fATK = 50.f;
		break;
	case E_HAMMER:
		m_Status.fATK = 30.f;
		break;
	case E_Claymore:
		m_Status.fATK = 25.f;
		break;
	default:
		m_Status.fATK = 10.f;
		break;
	}


	switch (m_iCurrentArmor)
	{
	case NONARMOR:
		m_Status.fDFS = 0.f;
		m_Status.fMaxHP = 100.f;
		break;
	case MERCENARY:
		m_Status.fDFS = 30.f;
		//m_Status.fMaxHP = 125.f;
		break;
	case CHAMPION:
		m_Status.fDFS = 60.f;
		//m_Status.fMaxHP = 150.f;
		break;
	default:
		m_Status.fDFS = 0.f;
		break;
	}

	if (m_Status.fCurHP > m_Status.fMaxHP)
		m_Status.fCurHP = m_Status.fMaxHP;

}

void CPlayer::Eat_Potion()
{
	if (m_pGameInstance->isKeyDown('E'))
	{
		if (m_Status.iNumPotion <= 0 || m_Status.fCurHP >= m_Status.fMaxHP)
			return;

		m_Status.fCurHP += 50.f;
		_vector Origin = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		XMVectorSetY(Origin, XMVectorGetY(Origin) + 0.5f);
		CParticleManager::GetInstance()->Create_Particle(72, Origin, nullptr);
		if (m_Status.fCurHP > m_Status.fMaxHP)
			m_Status.fCurHP = m_Status.fMaxHP;

		EatPotionSound();
		m_Status.iNumPotion -= 1;
	}
}

void CPlayer::Swift()
{
	CParticleManager::GetInstance()->Create_Particle(4, m_pTransformCom->Get_State(CTransform::STATE_POSITION), this);
	m_Status.fSpeed = 1.5f;
	m_pGameInstance->PlaySound_Z(_T("sfx_item_potionSwiftness-001_soundWave.ogg"), SOUND_EFFECT, fEffectVolume);
	m_IsSwift = true;
}

void CPlayer::Set_Hit(_bool _isHit, _bool _isHitStrong)
{
	isHit = _isHit;
	m_HitTimeRatio = 1.f;
	isHitStrong = _isHitStrong;
	
	if (_isHitStrong == true)
	{
		hitSound();
		m_iCurrentState = STATE_HIT;
		m_pTransformCom->Set_JumpState(true);
	}
}

void CPlayer::HammerAttack()
{
	list<CGameObject*> monster = m_pGameInstance->Get_Layer_Objects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	for (auto& iter : monster)
	{
		if (iter == nullptr || iter->Get_Dead())
			continue;
		CTransform* trans = static_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")));
		if (XMVectorGetX(XMVector3Length(trans->Get_State(CTransform::STATE_POSITION) -
			m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < 2.f)
			continue;
		else
			static_cast<CMonster*>(iter)->Set_Damage(10.f, 0);
	}


}


HRESULT CPlayer::Create_Item()
{
	CItem::ItemDesc itemdesc{};
	itemdesc.fData = 1;
	itemdesc.fRadius = 0.1f;
	itemdesc.RandomValue = RandomInt(0, 12);

	XMStoreFloat4(&itemdesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);

	return S_OK;
}

HRESULT CPlayer::Create_Emerald()
{
	CEmerald::EMERALDDESC eDesc{};
	XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	for(int i = 0; i< RandomInt(1,6); ++i)
		m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Emerald"), TEXT("Prototype_GameObject_Emerald"), &eDesc);
	return S_OK;
}

void CPlayer::Create_Weapon_All()
{
	CItem::ItemDesc itemdesc{};
	itemdesc.fData = 1;
	itemdesc.fRadius = 0.1f;
	
	XMStoreFloat4(&itemdesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	itemdesc.RandomValue = 2;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 3;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 4;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 5;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 6;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 7;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 8;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
	itemdesc.RandomValue = 9;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Item"), &itemdesc);
}

void CPlayer::Generate_Lazer()
{
	CLazer::LAZERDESC desc{};
	desc.fRotationPerSec = 90.f;
	desc.ParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	desc.vZSize = 4.f;
	desc.fStartRotation = 30.f;
	//Prototype_GameObject_Lazer
	CGameObject* Lazer = nullptr;
	for (int i = 0; i < 4; i++)
	{
		desc.fStartRotation = 0;
		Lazer = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Lazer"), &desc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), Lazer);
		desc.fStartRotation += 90.f;
	}
	
}

void CPlayer::SwordSwing()
{
}

void CPlayer::DoubleaxeSwing()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_doubleAxeSwingBase-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_doubleAxeSwingBase-002_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	}
}

void CPlayer::KatanaSwing()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaSwingAirSwooshArmAir-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaSwingAirStronkSwooshArmAir-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	}

	_int random2 = RandomInt(0, 1);
	switch (random2)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaUniqueSwing-002_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaUniqueSwing-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	}
	

}

void CPlayer::SpearSwing()
{
}

void CPlayer::ClaymoreSwing()
{
	m_pGameInstance->PlaySound_Z(_T("sfx_item_claymoreSwing-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
}

void CPlayer::AxeSwing()
{
}

void CPlayer::HammerSwing()
{
	_int RandomSound = RandomInt(0, 2);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_hammerSwing-001_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_hammerSwing-002_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_hammerSwing-003_soundWave.ogg"), SOUND_EFFECT, fSwingVolume);
		break;
	}
}

void CPlayer::EatPotionSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_potionDrink-001_soundWave.ogg"), SOUND_EFFECT, fEffectVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_potionDrink-002_soundWave.ogg"), SOUND_EFFECT, fEffectVolume);
		break;
	}
}

void CPlayer::RandomMonsterSummon()
{
	CMonster::MONSTER_DEC dec{};
	CGameObject* monster = nullptr;

	_int RandomPos = RandomInt(0, 5);
	switch (RandomPos)
	{
	case 0:
		dec.Pos = _float4(-158.04, 45.51, -36.70, 1.f);
		break;
	case 1:
		dec.Pos = _float4(-163.0, 45.75, -42.74, 1.f);
		break;
	case 2:
		dec.Pos = _float4(-158.02, 45.51, -47.59, 1.f);
		break;
	case 3:
		dec.Pos = _float4(-163.47, 45.51, -33.72, 1.f);
		break;
	case 4:
		dec.Pos = _float4(-157.97, 45.50, -31.73, 1.f);
		break;
	case 5:
		dec.Pos = _float4(-161.87, 45.51, -38.98, 1.f);
		break;
	}


	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);
	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Scale = 1.f;

	_int iRandomNumber = RandomInt(0, 2);
	switch (iRandomNumber)
	{
	case 0:
		monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Zombie"), &dec);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
		break;
	case 1:
		monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Vindicator"), &dec);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
		break;
	case 2:
		monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Creeper"), &dec);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);
		break;
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_EFFECT, 0.3f);

	
}

void CPlayer::SkeletonHorseSummon()
{
	CMonster::MONSTER_DEC dec{};
	CGameObject* monster = nullptr;
	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Scale = 1.2f;
	dec.Pos = _float4(-58.68, 30.50, -87.03, 1.f);
	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);

	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skeleton_Horse"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);

	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Pos = _float4(-59.79, 30.50, -97.52, 1.f);
	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);

	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skeleton_Horse"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);

	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Pos = _float4(-48.65, 30.50, -96.55, 1.f);
	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);

	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skeleton_Horse"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);

	dec.RotationAngle = XMConvertToRadians(RandomFloat(0.f, 360.f));
	dec.Pos = _float4(-46.10, 30.50, -86.08, 1.f);
	CParticleManager::GetInstance()->Create_Particle(50, XMLoadFloat4(&dec.Pos), nullptr);

	monster = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skeleton_Horse"), &dec);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), monster);

	m_pGameInstance->PlaySound_Z(_T("sfx_teleportSimple-001_soundWave.ogg"), SOUND_EFFECT, 0.4f);
	
}

void CPlayer::MakeFireWorks()
{
	_float RandomY = RandomFloat(47.f, 60.f);
	_int RandomintParticle = RandomInt(77, 80);

	_vector vPos1 = XMVectorSet(-97.52, RandomY, 48.34, 1.f);
	_vector vPos2 = XMVectorSet(-95.86, RandomY, 39.82, 1.f);
	_vector vPos3 = XMVectorSet(-103.01, RandomY, 40.66, 1.f);
	_vector vPos4 = XMVectorSet(-98.83, RandomY, 43.61, 1.f);

	_int RandomintPos = RandomInt(0, 3);
	switch (RandomintPos)
	{
	case 0:
		CParticleManager::GetInstance()->Create_Particle((_uint)RandomintParticle, vPos1, nullptr);
		m_pGameInstance->PlaySound_Z(_T("fireworks-1.wav"), SOUND_EFFECT, 0.3f);
		break;
	case 1:
		CParticleManager::GetInstance()->Create_Particle((_uint)RandomintParticle, vPos2, nullptr);
		m_pGameInstance->PlaySound_Z(_T("fireworks-2.wav"), SOUND_EFFECT, 0.3f);
		break;
	case 2:
		CParticleManager::GetInstance()->Create_Particle((_uint)RandomintParticle, vPos3, nullptr);
		m_pGameInstance->PlaySound_Z(_T("fireworks-3.wav"), SOUND_EFFECT, 0.3f);
		break;
	case 3:
		CParticleManager::GetInstance()->Create_Particle((_uint)RandomintParticle, vPos4, nullptr);
		m_pGameInstance->PlaySound_Z(_T("fireworks-4.wav"), SOUND_EFFECT, 0.3f);
		break;
	}
}

void CPlayer::DashSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_player_swirlings-005_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_player_swirlings-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
		break;
	}
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*		pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*		pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	for (auto& pPartObject : m_PartObjects)
		Safe_Release(pPartObject);

	for (auto& pPartWeapon : m_Weapons)
		Safe_Release(pPartWeapon);

	for (auto& pPartArmor : m_Armors[MERCENARY])
		Safe_Release(pPartArmor);
	for (auto& pPartArmor : m_Armors[CHAMPION])
		Safe_Release(pPartArmor);

	Safe_Release(m_ComBehavior);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	m_PartObjects.clear();
}
