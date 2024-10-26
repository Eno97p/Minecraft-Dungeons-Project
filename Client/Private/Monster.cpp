#include "stdafx.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"
#include "Circle.h"
#include "Item.h"
#include "ParticleManager.h"
#include "MonsterHP.h"
#include "Emerald.h"

_float fHitVolume = 0.3f;
HRESULT CMonster::Set_Update(MONSTER_DEC* _dec)
{
	m_MonsterDec = *_dec;

	m_pTransformCom->Set_Scale(m_MonsterDec.Scale, m_MonsterDec.Scale, m_MonsterDec.Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_MonsterDec.RotationAngle);
	_vector vPos = XMLoadFloat4(&m_MonsterDec.Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CMonster::Set_Pos(_vector _pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _pos);
}

_uint CMonster::Get_NumAnim()
{
	return m_pModelCom->Get_NumAnim();
}

void CMonster::Set_AnimIndex()
{
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC((_uint)m_Animindex, true));
}

_bool CMonster::InterSectFromWeapon(CCollider* pDstCollidercom)
{
	if (m_pColliderCom->Intersect(pDstCollidercom))
	{
		if (!m_firsthit)
			m_firsthit = true;
		m_HitTimeRatio = 1.f;
		m_pStatus->fHp -= 20.f;
		m_IsHit = true;

		return m_IsHit;
	}
	else
		return false;
}

_bool CMonster::InterSect(CCollider* pDstCollidercom)
{
	return m_pColliderCom->Intersect(pDstCollidercom);
}

void CMonster::MakeHitParticle(_uint iIndex)
{
	_float random = RandomFloat(0.f, 180.f);
	_float RandomSignint = RandomSign();
	switch (iIndex)
	{
	case 0:
		CParticleManager::GetInstance()->Create_Particle(20, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		break;
	case 1:
		CParticleManager::GetInstance()->Create_Particle(21, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		break;
	case 2:
		CParticleManager::GetInstance()->Create_Particle(22, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		break;
	case 3:
		if (RandomSignint > 0)
		{
			CParticleManager::GetInstance()->Create_Particle(28, m_pTransformCom->Get_State(CTransform::STATE_POSITION),
				m_pTransformCom->Get_State(CTransform::STATE_LOOK), random);
		}
		else
			CParticleManager::GetInstance()->Create_Particle(28, m_pTransformCom->Get_State(CTransform::STATE_POSITION),
				m_pTransformCom->Get_State(CTransform::STATE_RIGHT), random);
		break;
	case 4:
		CParticleManager::GetInstance()->Create_Particle(23, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		break;
	case 5:
		CParticleManager::GetInstance()->Create_Particle(68, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		break;

	}
}

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject{ rhs },
	m_MonsterDec{rhs.m_MonsterDec }

{
}

void CMonster::CollisionEachOther(_float fTimeDelta)
{
	list<class CGameObject*> monster = m_pGameInstance->Get_Layer_Objects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
	for (auto& iter : monster)
	{
		if (iter == nullptr || iter == this)
			continue;
		CTransform* trans = static_cast<CTransform*>(iter->Get_Component(TEXT("Com_Transform")));
		_vector vDir = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - trans->Get_State(CTransform::STATE_POSITION);

		if (true == m_pGameInstance->isIn_WorldFrustum(trans->Get_State(CTransform::STATE_POSITION)))
		{
			CCollider* col = static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")));
			if (InterSect(col))
			{
				m_pTransformCom->MoveDirection(vDir, fTimeDelta, m_pNavigationCom);
			}
		}
		else
			continue;

	}
}
HRESULT CMonster::Initialize(void * pArg)
{
	SetStatus();
	CGameObject::GAMEOBJECT_DESC	GameObjectDesc{};


	GameObjectDesc.fSpeedPerSec = m_pStatus->m_fSpeed;
	GameObjectDesc.fRotationPerSec = m_pStatus->m_fRotationpersec;
	
	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;
	
	m_pTransformCom->Set_Scale(((MONSTER_DEC*)pArg)->Scale, ((MONSTER_DEC*)pArg)->Scale, ((MONSTER_DEC*)pArg)->Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), ((MONSTER_DEC*)pArg)->RotationAngle);
	_vector vPos = XMLoadFloat4(&((MONSTER_DEC*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_MonsterDec = *((MONSTER_DEC*)pArg);
	m_pPlayer = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
	if (m_pPlayer == nullptr)
		return E_FAIL;

	m_pPlayerCollider = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
	m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	m_fFrameChange = rand() % 3;

	m_pParticleManager = CParticleManager::GetInstance();

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	CMonsterHpBar::CMonsterHPDesc hpdesc{};
	hpdesc.CurrentHp = &m_pStatus->fHp;
	hpdesc.MaxHp = &m_pStatus->fMaxHp;
	hpdesc.ParentMat = m_pTransformCom->Get_WorldFloat4x4();

	m_SmallHpBar = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MonsterHPBar"), &hpdesc);

	return S_OK;
}
void CMonster::Tick(_float fTimeDelta)
{
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

	if (!m_bIsDetected)
	{
		m_fAcctime += fTimeDelta;
		if (m_fAcctime > m_fFrameChange)
		{
			m_fAcctime = 0.f;
			m_fFrameChange = RandomFloat(0.f,3.f);
			if (m_bIdle)
				m_bIdle = false;
			else
				m_bIdle = true;
		}
	}

	if (m_IsHit == true)
	{
		m_HitReset -= fTimeDelta;
		if (m_HitReset <= 0.f)
		{
			m_IsHit = false;
			m_HitReset = 0.2f;
		}
	}

	if (m_HitTimeRatio > 0.0f)
	{
		m_HitTimeRatio -= fTimeDelta / 0.4f;
		if (m_HitTimeRatio < 0.0f) {
			m_HitTimeRatio = 0.0f;
		}
	}


	
}
void CMonster::LookAtPlayer()
{
	_vector PlayerPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->LookAt_For_LandObject(PlayerPosition);
}
_vector CMonster::Calculate_Player_Dir()
{
	CTransform* PlayerTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_Transform")));
	_vector PlayerPosition = PlayerTransform->Get_State(CTransform::STATE_POSITION);

	_vector MyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vDir = PlayerPosition - MyPosition;
	
	vDir = XMVector4Normalize(vDir);
	return vDir;
}
HRESULT CMonster::AddNavigation()
{
	CNavigation::NAVIGATION_DESC	NavigationDesc{};

	NavigationDesc.iCurrentCellIndex = 0;
	NavigationDesc.vPos = &m_MonsterDec.Pos;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
		return E_FAIL;

	return S_OK;
}
HRESULT CMonster::Render_LightDepth()
{


	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* ±¤¿ø ±âÁØÀÇ ºä º¯È¯Çà·Ä. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(68.f, 150.f, -126.f, 0.f), XMVectorSet(83.f, 24.f, -36.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}

	return S_OK;
}
void CMonster::Set_Damage(_float fDamage, _uint ParticleNum)
{
	if (m_IsHit == true)
		return;
	switch (ParticleNum)
	{
	case 0:	//Ä®
		SwordHitSound();
		MakeHitParticle(0);
		MakeHitParticle(2);
		break;
	case 1: //´õºí¾×½º
		AxeHitSound();
		MakeHitParticle(3);
		MakeHitParticle(0);
		break;
	case 2: //Ã¢
		SpearHitSound();
		MakeHitParticle(4);
		MakeHitParticle(2);
		break;
	case 3: //µµ³¢
		AxeHitSound();
		MakeHitParticle(3);
		MakeHitParticle(0);
		break;
	case 4:		//Katana
		KatanaHitSound();
		MakeHitParticle(1);
		MakeHitParticle(5);
		break;
	case 6:		//Claymore
		ClayMoreHitSound();
		MakeHitParticle(3);
		MakeHitParticle(0);
		break;
	default:
		break;
	}

	HurtSound();
	m_HitTimeRatio = 1.f;
	m_pStatus->fHp -= fDamage;
	m_IsHit = true;
	if (!m_firsthit)
		m_firsthit = true;

}
_vector CMonster::PlayerPos()
{
	return m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
}
_bool CMonster::IsPlayerApproach(_float fDistance)
{
	if (XMVectorGetX(XMVector4Length(m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) -
		m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < fDistance)
		return true;
	else
		return false;
}
void CMonster::MakeCircle(_float fRadius, _float fLifeTime, _uint ParticleNumber, CGameObject* pTarget)
{
	CCircle::CIRCLEDESC circle = {};
	circle.fRadius = fRadius;
	circle.LifeTimeMax = fLifeTime;
	circle.ParticleNum = ParticleNumber;
	if (pTarget == nullptr)
	{
		XMStoreFloat4(&circle.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else
	{
		CTransform* trans = static_cast<CTransform*>(pTarget->Get_Component(TEXT("Com_Transform")));
		XMStoreFloat4(&circle.vPos, trans->Get_State(CTransform::STATE_POSITION));
	}
	CGameObject* Circle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Circle"), &circle);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Circle"), Circle);

	//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Circle"), TEXT("Prototype_GameObject_Circle"), &circle);
}
void CMonster::RandomCircle(_float fRadius, _float fLifeTime, _uint ParticleNumber, _float fDistance)
{
	CCircle::CIRCLEDESC circle = {};
	circle.fRadius = fRadius;
	circle.LifeTimeMax = fLifeTime;
	circle.ParticleNum = ParticleNumber;
	
	circle.vPos = m_pNavigationCom->Get_Random_Cell_Point(fDistance);

	CGameObject* Circle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Circle"), &circle);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Circle"), Circle);

}
void CMonster::TurnToPlayer(_float fTimeDelta)
{

	m_pTransformCom->TurnToTarget(fTimeDelta, m_pPlayerTransform->Get_State(CTransform::STATE_POSITION));

}
NodeStates CMonster::Detect(_float fTimeDelta)
{
	if (IsPlayerApproach(m_DetectDist))
	{
		m_bIsDetected = true;
		return SUCCESS;
	}
	else
	{
		m_bIsDetected = false;
		return FAILURE;
	}
}

void CMonster::HurtSound()
{
}

HRESULT CMonster::Create_Item(_uint itemIndex)
{
	CItem::ItemDesc itemdesc{};
	itemdesc.fRadius = 0.1f;
	itemdesc.RandomValue = itemIndex;
	XMStoreFloat4(&itemdesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CGameObject* Item = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &itemdesc);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), Item);

	return S_OK;
}

HRESULT CMonster::RandomDrop()
{
	_int RandomNumber = RandomInt(0, 9);
	_int RandomPotion = RandomInt(0, 1);
	_int RandomMoney = RandomInt(0, 10);
	if (RandomNumber == 0)
	{
		Create_Item(RandomPotion);
	}
	else if (RandomNumber == 5)
	{
		Create_Item(12);
	}
	Create_Emerald(RandomMoney);
	return S_OK;
}

HRESULT CMonster::NamedDrop()
{
	Create_Item(12);
	_int RandomPotion = RandomInt(0, 1);
	Create_Item(RandomPotion);
	_int RandomMoney = RandomInt(10, 30);
	Create_Emerald(RandomMoney);
	return S_OK;
}

HRESULT CMonster::Create_Emerald(_uint iNumEmerald)
{
	CEmerald::EMERALDDESC eDesc{};
	XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CGameObject* Cemerald = nullptr;
	for (int i = 0; i < iNumEmerald; ++i)
	{
		Cemerald = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Emerald"), &eDesc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Emerald"), Cemerald);
	}
	return S_OK;
}

void CMonster::SwordHitSound()
{
	_int RandomSound = RandomInt(0, 3);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-002_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-003_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_misc_swordHit-004_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	}

}

void CMonster::AxeHitSound()
{
	_int RandomSound = RandomInt(0, 4);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_axeImpact-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_axeImpact-002_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 2:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_axeImpact-003_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 3:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_axeImpact-004_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 4:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_axeImpact-005_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	}

}

void CMonster::ClayMoreHitSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_claymoreHitSlashMeat-002_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_claymoreHitStabMeat-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	}

}

void CMonster::KatanaHitSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaUniqueSwing-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_katanaUniqueSwing-002_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_item_spearHitMediumFlesh-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
}

void CMonster::SpearHitSound()
{
	_int RandomSound = RandomInt(0, 1);
	switch (RandomSound)
	{
	case 0:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_spearSwingLightMetalAndSwoosh-002_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	case 1:
		m_pGameInstance->PlaySound_Z(_T("sfx_item_spearSwingLightMetalAndSwoosh-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);
		break;
	}

	m_pGameInstance->PlaySound_Z(_T("sfx_item_spearHitMediumFlesh-001_soundWave.ogg"), SOUND_EFFECT, fHitVolume);

}

void CMonster::Free()
{
	__super::Free();
	Safe_Delete(m_pStatus);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_ComBehavior);
	Safe_Release(m_SmallHpBar);
}
