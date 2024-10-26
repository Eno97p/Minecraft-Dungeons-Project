
#include "..\Public\Loader.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "Creeper.h"
#include "Zombie.h"
#include "MapTest.h"
#include "ForkLift.h"
#include "Fiona.h"
#include "Skeleton.h"
#include "Skeleton_Horse.h"
#include "Enderman.h"
#include "Player.h"
#include "NPC.h"
#include "Golem.h"
#include "RedstoneBoss.h"
#include "Geomancer.h"
#include "Vindicator.h"
#include "Archillager.h"
#include "HeartOfEnder.h"
#include "Husk.h"
#include "Map_Flat.h"
#include "QuarterCamera.h"
#include "ItemBox.h"
#include "Door.h"
#include "Chair.h"

#include "Particle_Point.h"
#include "ParticleMesh.h"
#include "Particle_Rect.h"

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
	case LEVEL_EFFECT:
		hr = Loading_For_EffectLevel();
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
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Tile%d.dds"), 2))))
		return E_FAIL;
	
	lstrcpy(m_szLoadingText, TEXT("모델를(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/Terrain/Height1.bmp")))))
		return E_FAIL;

	_matrix		PreTransformMatrix;

	/* For.Prototype_Component_Model_ForkLift */

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MapTest"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/MapTest/Map0.fbx", PreTransformMatrix))))
		return E_FAIL;



	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FirstStage"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/RealMap/map1.fbx", PreTransformMatrix))))
		return E_FAIL;



	/* For.Prototype_Component_Model_Fiona */
	PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fiona"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Fiona/Fiona.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Creeper */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Creeper"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/SK_Creeper/Creeper.fbx", PreTransformMatrix))))
		return E_FAIL;

	/* For.Prototype_Component_Model_Zombie */
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Zombie"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Zombie/Zombie.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Jugglus"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Andras_0724/Andras.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Wander/Wander.fbx", PreTransformMatrix))))
		return E_FAIL;




	//NPC
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_NPC"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/NPC/NPC.fbx", PreTransformMatrix))))
		return E_FAIL;

	//ItemBox
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/ItemBox/ItemBox.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Door
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/Door/Door.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Chair
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Chair"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/Chair/Chair.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Andras_Lazer_Base"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/AndrasLazer/AndrasLazerQuarter.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/SK_Skeleton/Skeleton.fbx", PreTransformMatrix))))
		return E_FAIL;

	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) /** XMMatrixRotationY(XMConvertToRadians(180.0f))*/;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Skeleton_Horseman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/SK_SkeletonHorseman/Horseman.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Player
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/Alex/PlayerFinal.fbx", PreTransformMatrix))))
		return E_FAIL;

	//enderman
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enderman"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../Bin/Resources/Models/SK_Enderman/Enderman.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Golem
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneGolem"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/SK_RedStoeneGolem/RedstoneGolem.fbx", PreTransformMatrix))))
		return E_FAIL;

	//RedStoneBoss
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RedstoneBoss"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/RedStoneBoss/RedstoneBoss.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Geomancer
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Geomancer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Geomancer/Geomanver.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Vindicator
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Vindicator"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Vindicator/Vindicator.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Enchanter
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enchanter"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Enchanter/Enchanter.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Wraith
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Wraith"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Wraith/Wraith.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Necromancer
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Necromancer"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Necromancer/Necromancer.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Archillager 최종보스
	PreTransformMatrix = XMMatrixScaling(0.005f, 0.005f, 0.005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Archillager"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Archillager/Archillager.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Heart_Of_Ender"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/HeartOfEnder/HeartOfEnder.fbx", PreTransformMatrix))))
		return E_FAIL;

	//Husk
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Husk"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, "../../Client/Bin/Resources/Models/Actors/Husk/Husk.fbx", PreTransformMatrix))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("네비게이션(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		CNavigation::Create(m_pDevice, m_pContext/*, TEXT("../Bin/BinaryFile/Navigation.dat")*/))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Creeper */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Creeper"),
		CCreeper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_Creeper */
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


	//Forklift
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ForkLift"),
		CForkLift::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	//Fiona
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fiona"),
		CFiona::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	//Enderman
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Enderman"),
		CEnderman::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//PlayerTest
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Golem
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Golem"),
		CGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//RedstoneBoss
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RedstoneBoss"),
		CRedstoneBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Geomancer
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Geomancer"),
		CGeomancer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Vindicator
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Vindicator"),
		CVindicator::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Archillager
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Archillager"),
		CArchillager::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Heart_Of_Ender
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Heart_Of_Ender"),
		CHeartOfEnder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Husk
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Husk"),
		CHusk::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_MapTest */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_MapTest"),
		CMapTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;



	/* For.Prototype_GameObject_NPC */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_NPC"),
		CNPC::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_ItemBox */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ItemBox"),
		CItemBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_GameObject_Door */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	/* For.Prototype_GameObject_Chair */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Chair"),
		CChair::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_For_EffectLevel()
{

	lstrcpy(m_szLoadingText, TEXT("로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Texture_Emerald"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Textures/UI/icon_emerald.png"), 1))))
		return E_FAIL;




	_matrix		PreTransformMatrix;

	PreTransformMatrix = XMMatrixScaling(1.f, 1.f, 1.f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Model_Slash"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/Slash/Slash.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Model_Cube"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/NewCube/NewCube.fbx", PreTransformMatrix))))
		return E_FAIL;

	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Model_Circle"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../../Client/Bin/Resources/Models/ParticleCircle/ParticleCircle.fbx", PreTransformMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Model_Fork"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/ForkLift/ForkLift.fbx", PreTransformMatrix))))
		return E_FAIL;


	PreTransformMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Model_Flat"),
		CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, "../Bin/Resources/Models/Flat/Flat.fbx", PreTransformMatrix))))
		return E_FAIL;



	lstrcpy(m_szLoadingText, TEXT("셰이더를(을) 로딩 중 입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxAnimMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_InstanceRect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxInstance_Rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Rect.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_InstanceMesh */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Mesh.hlsl"), VTXINSTANCE_RECT::Elements, VTXINSTANCE_RECT::iNumElements))))
		return E_FAIL;


	/* For.Prototype_Component_Shader_InstancePoint */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_Shader_VtxInstance_Point"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxInstance_Point.hlsl"), VTXINSTANCE_POINT::Elements, VTXINSTANCE_POINT::iNumElements))))
		return E_FAIL;


	
	CVIBuffer_Instance::INSTANCE_DESC		InstanceDesc{};
	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.iNumInstance = 1000;
	InstanceDesc.vOffsetPos = _float3(0.0f, 0.f, 0.0f);
	InstanceDesc.vPivotPos = _float3(0.0f, 30.f, 0.0f);
	InstanceDesc.vRange = _float3(500.0f, 0.5f, 500.0f);
	InstanceDesc.vSize = _float2(0.2f, 0.4f);
	InstanceDesc.vSpeed = _float2(1.f, 7.f);
	InstanceDesc.vLifeTime = _float2(10.f, 15.f);
	InstanceDesc.isLoop = true;
	InstanceDesc.vGravity = _float2(1.f, 2.f);

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_VIBuffer_Instance_Point"),
		CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;


	ZeroMemory(&InstanceDesc, sizeof InstanceDesc);

	InstanceDesc.iNumInstance = 300;
	InstanceDesc.vOffsetPos = _float3(0.0f, -3.f, 0.0f);
	InstanceDesc.vPivotPos = _float3(0.0f, 0.f, 0.0f);
	InstanceDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
	InstanceDesc.vSize = _float2(0.2f, 0.4f);
	InstanceDesc.vSpeed = _float2(1.f, 7.f);
	InstanceDesc.vLifeTime = _float2(1.f, 3.f);
	InstanceDesc.isLoop = false;
	InstanceDesc.vGravity = _float2(1.f, 2.f);


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_EFFECT, TEXT("Prototype_Component_VIBuffer_Instance_Rect"),
		CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FlatMap"),
		CFlatMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_QuarterCamera"),
		CQuarterCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticlePoint"),
		CParticle_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleMesh"),
		CParticleMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_ParticleRect"),
		CParticle_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


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
