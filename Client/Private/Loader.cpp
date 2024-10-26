#include "stdafx.h"
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "Monster.h"
#include "Player.h"
#include "Body_Player.h"
#include "MapTest.h"

#pragma region WEAPON
#include "Weapon.h"
#include "Sword.h"
#include "Bow.h"
#include "DoubleAxe.h"
#include "Axe.h"
#include "Spear.h"
#include "Arrow.h"
#include "Katana.h"
#include "Claymore.h"
#include "Hammer.h"
#pragma endregion

#pragma region ARMOR
#include "MA_Body.h"
#include "MA_Head.h"
#include "MA_L_Arm.h"
#include "MA_R_Arm.h"
#include "MA_L_Leg.h"
#include "MA_R_Leg.h"

#include "Cham_Body.h"
#include "Cham_Head.h"
#include "Cham_L_Arm.h"
#include "Cham_R_Arm.h"
#pragma endregion





#pragma region MONSTER
#include "Creeper.h"
#include "Skeleton.h"
#include "Skeleton_Horse.h"
#include "Zombie.h"
#include "Golem.h"
#include "RedStoneBoss.h"
#include "Fiona.h"
#include "ForkLift.h"
#include "Vindicator.h"
#include "Archillager.h"
#include "Visage.h"
#include "HeartOfEnder.h"
#pragma endregion

#pragma region ENVIRONMENT
#include "NPC.h"
#include "AttackBox.h"
#include "Circle.h"
#include "Item.h"
#include "Emerald.h"
#include "Sky.h"
#include "ItemBox.h"
#include "Door.h"
#include "Chair.h"
#pragma endregion ENVIRONMENT

#pragma region UI
#include "BossHpBar.h"
#include "Mouse.h"
#include "Inven_Frame.h"
#include "Inven_MainSlot.h"
#include "Inven_ItemIcon.h"
#include "Inven_Item_Info.h"
#include "Emerald_UI.h"
#include "DummyPlayer.h"
#include "HotBar_Frame.h"
#include "HPBarFrame.h"
#include "HPBar.h"
#include "DashIcon.h"
#include "HotBarSlot.h"
#include "HotBarIcon.h"
#include "ItemRarity.h"
#include "ArrowSlot.h"
#include "Shop_Frame.h"
#include "Shop_Slot.h"
#include "Shop_Icon.h"
#include "Shop_Info.h"
#include "ShopRarity.h"
#include "ShopPrice.h"
#include "ShopNomoney.h"
#include "FKey.h"
#include "MonsterHP.h"
#include "HUD.h"
#include "EpicBeam.h"
#include "EpicGround.h"
#include "EpicPillar.h"
#pragma endregion UI

#pragma region EFFECT
#include "Core.h"
#include "Core_Bomb.h"
#include "ParticleMesh.h"
#include "RSS.h"
#include "Lazer.h"
#pragma endregion EFFECT


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	CLoader*		pLoader = (CLoader*)pArg;

	if (FAILED(pLoader->Loading()))
		return 1;

	CoUninitialize();

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{


	EnterCriticalSection(&m_Critical_Section);

	HRESULT			hr{};

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_Critical_Section);

	if (FAILED(hr))
		return E_FAIL;


	return S_OK;
}

HRESULT CLoader::Loading_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));


	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));	


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩 중 입니다."));

#pragma region TEXTURE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;

	//전조패턴 원
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Circle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/RedCircle.png"), 1))))
		return E_FAIL;

	//보스 HP바
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHPBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/BossBar/BossHPBar.png"), 1))))
		return E_FAIL;

	//인벤토리 프레임
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inven_Frame.png"), 1))))
		return E_FAIL;

	//인벤토리 메인슬롯
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenMainSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/item_main_slot.png"), 1))))
		return E_FAIL;

	//핫바 메인슬롯
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hot_Bar_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/bigslot.png"), 1))))
		return E_FAIL;

	//화살 슬롯
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ArrowSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/octagon_frame.png"), 1))))
		return E_FAIL;

	//아이템 : 에메랄드
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Emerald"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/icon_emerald.png"), 1))))
		return E_FAIL;

	//아이템 : 포션
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_StrengthPotion_Icon.png"), 1))))
		return E_FAIL;

	//아이템 : 신속포션
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Swift_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_SwiftnessPotion_Icon.png"), 1))))
		return E_FAIL;

	//아이템 : 카타나
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Katana"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Katana_Unique2_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 해머
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Hammer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Hammer_Unique1_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 클레이모어
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Claymore"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Claymore_Unique3_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 더블액스
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_DoubleAxe"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_DoubleAxe_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 도끼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Axe"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Axe_Unique2_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 창
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Spear"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Spear_Unique2_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 머셔너리아머
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_MercenaryArmor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_MercenaryArmor_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 챔피언 아머
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_ChampionArmor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_ChampionsArmor_Unique1_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 기본 칼
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Sword"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Sword_Steel_Icon_inventory.png"), 1))))
		return E_FAIL;

	//아이템 : 기본 활
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Bow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Bow_Icon_inventory.png"), 1))))
		return E_FAIL;

	//상태창 프레임
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HotBarFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MPhotbar_background_BOTTOM_switch.png"), 1))))
		return E_FAIL;

	//HP 프레임
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Frame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HEARTFRAME.png"), 1))))
		return E_FAIL;

	//HP 기본
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Default"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Heart%d.png"), 2))))
		return E_FAIL;

	//HP 데미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Damage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/heart_damage.png"), 1))))
		return E_FAIL;

	//대쉬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DashIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/rolling_icon.png"), 1))))
		return E_FAIL;

	//레어리티
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rarity"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Rarity%d.png"), 3))))
		return E_FAIL;

	//인벤토리 아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/v2_icon_inventory.png"), 1))))
		return E_FAIL;

	//포션 아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Potion"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_HealthPotion_Icon_inventory.png"), 1))))
		return E_FAIL;

	//화살 아이콘
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ArrowIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/T_Arrow_Icon_Inventory.png"), 1))))
		return E_FAIL;

	//상점 프레임 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopFrame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NewShopFrame.png"), 1))))
		return E_FAIL;

	//상점 버튼 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/inputbutton_wide.png"), 1))))
		return E_FAIL;

	//상점 슬롯
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ShopSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/frame_buff.png"), 1))))
		return E_FAIL;

	//버튼프레임 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FontButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FKey.png"), 1))))
		return E_FAIL;

	//노이즈 마스크 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Noise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Noise/Noise%d.png"), 16))))
		return E_FAIL;

	//HUD 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HUD.png"), 1))))
		return E_FAIL;

	//에픽빔

	//기둥
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Pillar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/beam/%d.png"), 16))))
		return E_FAIL;

	//빔
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/lightbar/%d.png"), 11))))
		return E_FAIL;

	//루프1
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Loop1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/emberhell/%d.png"), 40))))
		return E_FAIL;

	//루프2
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Loop2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/drop_after_loop/%d.png"), 12))))
		return E_FAIL;

	//기둥 파티클
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Pillar2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/bookmark_beamparticlevertical/%d.png"), 20))))
		return E_FAIL;

	//기둥 파티클2
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Epic_Pillar3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/EpicBeam/bookmark_beamparticlehelix/%d.png"), 20))))
		return E_FAIL;

#pragma endregion TEXTURE
	
	lstrcpy(m_szLoadingText, TEXT("트리(을) 로딩 중 입니다."));

#pragma region BEHAVIORTREE
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_BehaviorTree"),
		CBehaviorTree::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion BEHAVIORTREE

	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));

#pragma region MODEL
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	_matrix		PreTransformMatrix;


	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/NPC/NPC.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_ForkLift */
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	//SkySphere
	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SkyBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/SkySphere/SkySphere.fbx", PreTransformMatrix))))
		return E_FAIL;



	/* For.Prototype_Component_Model_Emerald */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Emerald"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Emerald/Emerald.fbx", PreTransformMatrix))))
		return E_FAIL;


	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Creeper */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Creeper"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_Creeper/Creeper.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Spider
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spider"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_Spider/Spider.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Skeleton
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_Skeleton/Skeleton.fbx", PreTransformMatrix))))
		return E_FAIL;

	//SkeletonHorseman
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton_Horseman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_SkeletonHorseman/Horseman.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Enderman
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enderman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_Enderman/Enderman.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Zombie
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zombie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Zombie/Zombie.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Golem
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneGolem"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/SK_RedStoeneGolem/RedstoneGolem.fbx", PreTransformMatrix))))
		return E_FAIL;

	//RedStoneBoss
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneBoss"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/RedStoneBoss/RedstoneBoss.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Geomancer
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Geomancer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Geomancer/Geomanver.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Vindicator
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Vindicator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Vindicator/Vindicator.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Enchanter
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchanter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Enchanter/Enchanter.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Wraith
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wraith"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Wraith/Wraith.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Necromancer
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Necromancer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Necromancer/Necromancer.fbx", PreTransformMatrix))))
		return E_FAIL;


	//Archillager 최종보스
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Archillager"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Archillager/Archillager.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Heart Of Ender
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HeartOfEnder"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/HeartOfEnder/HeartOfEnder.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Heart Of Ender Visage
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Visage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/HeartOfEnder/Visage.fbx", PreTransformMatrix))))
		return E_FAIL;


	//Player
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Actors/Alex/PlayerFinal.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ItemBox
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/ItemBox/ItemBox.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Door
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Door/Door.fbx", PreTransformMatrix))))
		return E_FAIL;


	//Chair
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Chair/Chair.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion MODEL
	
#pragma region MODEL_WEAPON
	/* sword */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sword"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Sword/Sword.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* doubleaxe */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f)* XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DoubleAxe"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/DoubleAxe/DoubleAxe.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Bow */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Bow/Bow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Arrow */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Arrow"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/SK_Arrow/Arrow.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Spear */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Spear"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Spear/Spear.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Axe */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Axe"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Axe/Axe.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Katana */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Katana"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Katana/Katana.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Claymore */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Claymore"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Claymore/Claymore.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* Hammer */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Hammer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Weapons/Hammer/Hammer.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion MODEL_WEAPON

#pragma region MODEL_ARMOR
	/*================================================== Mercenary ================================================================== */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_Body"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/MercenaryArmor.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_Head"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/MA_Helmet.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_L_Leg"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/MA_L_Leg.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_L_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/Ma_L_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_R_Leg"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/MA_R_Leg.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MA_R_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/MercenaryArmor/MA_R_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;

	/*================================================== Champions ================================================================== */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cham_Body"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/ChampionsArmor/Cham_Body.fbx", PreTransformMatrix))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cham_Head"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/ChampionsArmor/Cham_Head.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cham_L_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/ChampionsArmor/Cham_L_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cham_R_Arm"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Armors/ChampionsArmor/Cham_R_Arm.fbx", PreTransformMatrix))))
		return E_FAIL;


#pragma endregion MODEL_ARMOR
	
#pragma region MODEL_EFFECT
	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Cube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/NewCube/NewCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ParticleCircle/ParticleCircle.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Core"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Core/Core.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Slash/Slash.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f)* XMMatrixRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lazer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Lazer/LazerModel.fbx", PreTransformMatrix))))
		return E_FAIL;

#pragma endregion MODEL_EFFECT

#pragma region MAP
	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MapTest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/RealMap/Map1.fbx", PreTransformMatrix))))
		return E_FAIL;
#pragma endregion MAP

	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));

#pragma region SHADER
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh_Old"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh_Old.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	/* 맵 전용 셰이더 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Map"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Map.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_BossHpBar */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossHpBar"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_BossHPBar.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_UI */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ForUI.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Billboard */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Bill"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PosTexBillBoard.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;



	/* For.Prototype_Component_Shader_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Circle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Circle.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Sky */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Sky"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Sky.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	//인스턴스 메쉬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance_Mesh.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	//이펙트 큐브용 메쉬
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_CubeTexture"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cube_Color.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;



#pragma endregion SHADER

	lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));

#pragma region NAVIGATION
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, "../Bin/BinaryFile/Navigation.dat"))))
		return E_FAIL;
#pragma endregion NAVIGATION

	lstrcpy(m_szLoadingText, TEXT("충돌체 원형을 로딩 중 입니다."));

#pragma region COLLIDER
	
	/* For.Prototype_Component_Collider */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;
#pragma endregion COLLIDER

	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));

#pragma region CLASS_ENVIRONMENT
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Circle"),
		CCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapTest"),
		CMapTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item"),
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Emerald"),
		CEmerald::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Emerald_UI"),
		CEmerald_UI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inven_Icon"),
		CInven_ItemIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inven_Item_Info"),
		CInven_ItemINFO::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//ItemBox
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemBox"),
		CItemBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Door
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Chair
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chair"),
		CChair::Create(m_pDevice, m_pContext))))
		return E_FAIL;
#pragma endregion CLASS_ENVIRONMENT
	
#pragma region CLASS_ACTORS
	/* For.Prototype_GameObject_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Body_Player */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Creeper
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Creeper"),
		CCreeper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Zombie */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Zombie"),
		CZombie::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Skeleton */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skeleton"),
		CSkeleton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SkeletonHorse */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skeleton_Horse"),
		CSkeletonHorse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Golem */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Golem"),
		CGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_RedStoneBoss */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RedStoneBoss"),
		CRedStoneBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Vindicator*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vindicator"),
		CVindicator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Archillager*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Archillager"),
		CArchillager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Visage*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Visage"),
		CVisage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_HeartOfEnder*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HeartOfEnder"),
		CHeartOfEnder::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_NPC */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC"),
		CNPC::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Fiona */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fiona"),
		CFiona::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_ForkLift */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	//AttackBox
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AttackBox"),
		CAttackBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion CLASS_ACTORS

#pragma region CLASS_WEAPON
	/* For.Prototype_GameObject_Weapon */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sword"),
		CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Bow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bow"),
		CBow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_DoubleAxe */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DoubleAxe"),
		CDoubleAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Axe */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Axe"),
		CAxe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Spear */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Spear"),
		CSpear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Arrow */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Arrow"),
		CArrow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Katana */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Katana"),
		CKatana::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Hammer */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Hammer"),
		CHammer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Claymore */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Claymore"),
		CClaymore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion CLASS_WEAPON

#pragma region CLASS_ARMOR

	/* ========================================== Mercenary ============================================*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_Body"),
		CMA_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_Head"),
		CMA_Head::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_L_Leg"),
		CMA_L_L::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_L_Arm"),
		CMA_L_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_R_Leg"),
		CMA_R_Leg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MA_R_Arm"),
		CMA_R_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/*============================================Champions===============================================*/

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cham_Head"),
		Cham_Head::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cham_Body"),
		Cham_Body::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cham_L_Arm"),
		Cham_L_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Cham_R_Arm"),
		Cham_R_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion CLASS_ARMOR

#pragma region CLASS_EFFECT
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Core"),
		CCore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Core_Bomb"),
		CCore_Bomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Mesh"),
		CParticleMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RSS"),
		RSS::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Lazer"),
		CLazer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion CLASS_EFFECT

	lstrcpy(m_szLoadingText, TEXT("UI를 로딩 중 입니다."));
#pragma region CLASS_UI

	//BossHPBar
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossHPBar"),
		CBossHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Inven Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenFrame"),
		CInven_Frame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Inven MainSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenMainSlot"),
		CInven_MainSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Inven MainSlot */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dummyplayer"),
		CDummyPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Hot Bar Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HotBarFrame"),
		CHotBarFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* HP Bar Frame */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPBarFrame"),
		CHPBarFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* HP Bar*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HPBar"),
		CHPBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Dash Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DashIcon"),
		CDashIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Hot Bar Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HotBarSlot"),
		CHotBarSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Hot Bar Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HotBarIcon"),
		CHotBarIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Rarity*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Item_Rarity"),
		CItemRarity::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Arrow Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Arrow_Slot"),
		CArrowSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Frame*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Frame"),
		CShopFrame::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Slot*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Slot"),
		CShopSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Icon*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Icon"),
		CShopIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Info*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Info"),
		CShop_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Rarity*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Rarity"),
		CShopRarity::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Price*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Price"),
		CShopPrice::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* Shop Nomoney*/
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Shop_Nomoney"),
		CShopNomoney::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* FKey */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FKey"),
		CFKey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* FKey */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MonsterHPBar"),
		CMonsterHpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* HUD */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_HUD"),
		CHUD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* EpicBeam */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EpicBeam"),
		CEpicBeam::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* EpicBeam2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EpicGround"),
		CEpicGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* EpicPillar */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EpicPillar"),
		CEpicPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion CLASS_UI

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVEL eNextLevel)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed To Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_Critical_Section);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
