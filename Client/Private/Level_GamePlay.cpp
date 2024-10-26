#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "Layer.h"
#include "Map.h"
#include "Player.h"
#include "Monster.h"
#include "Environment.h"
#include "UIManager.h"
#include "Inventory.h"
#include "CollisionMgr.h"

#include "ParticleManager.h"
CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	/*ShowCursor(false);*/
	CUIManager::GetInstance()->Initialize();

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Map(TEXT("Layer_Map"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Mouse(TEXT("Layer_Mouse"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;
	
	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;
	m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Prototype_GameObject_Sky"));



	if (FAILED(Camera_Setting()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	//m_pGameInstance->PlayBGM(_T("Bgm.OGG"), 0.3f);
	//m_pGameInstance->PlaySubBGM(_T("WindLoop.wav"), 0.3f);
	CParticleManager::GetInstance()->Initialize();
	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
#endif
	CInventory::GetInstance()->Tick(fTimeDelta);
	CUIManager::GetInstance()->Tick(fTimeDelta);
	CollisionMgr::GetInstance()->Tick(fTimeDelta);

	if (!m_fStarted)
	{
		AccTime += fTimeDelta;
		if (!BgmOn)
		{
			m_pGameInstance->PlaySound_Z(_T("vo_cin_hihaIntro-001.OGG"),SOUND_EFFECT, 0.4f);
			m_pGameInstance->PlaySound_Z(_T("bgm_env_hiha2DMainIntro-001_soundWave.OGG"), SOUND_PLAYER, 0.3f);
			
			CGameObject* Archillegar = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), "Fiona");
			static_cast<CFreeCamera*>(m_pCamera)->CutScene(Archillegar);
			BgmOn = true;
		}
		
		
		if (AccTime > 19.f)
		{
			m_fStarted = true;
			m_pGameInstance->PlayBGM(_T("SubBgm.wav"), 0.3f);
			static_cast<CFreeCamera*>(m_pCamera)->Set_CutSceneEnd();
			static_cast<CFreeCamera*>(m_pCamera)->Set_Target(m_pPlayer);
			static_cast<CFreeCamera*>(m_pCamera)->Reset_Distance();
		}
		else
		{
			static_cast<CFreeCamera*>(m_pCamera)->Distance_Up(fTimeDelta * 2.f);
			static_cast<CFreeCamera*>(m_pCamera)->Angle_Down(fTimeDelta);
		}
	}

	

}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CFreeCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0, 10.0f, -20.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);
	
	if (FAILED(m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &m_pCamera, &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Mouse(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring& strLayerTag)
{
	string finalPath = "../Bin/BinaryFile/Save_Player.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		CPlayer::PLAYERDEC dec = {};
		inFile.read((char*)&dec, sizeof(CPlayer::PLAYERDEC));
		GetGI->Add_CloneObjectZ(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"),&m_pPlayer, &dec);
	}
	inFile.close();




	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"))))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	string finalPath = "../Bin/BinaryFile/SaveData.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		MSG_BOX("No File");
		return S_OK;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		size_t objNum;
		inFile.read((char*)&objNum, sizeof(size_t));
		for (size_t i = 0; i < objNum; ++i)
		{
			CMonster::MONSTER_DEC Dec = {};
			inFile.read((char*)&Dec, sizeof(CMonster::MONSTER_DEC));
			CEnvironment::Envi_Desc eDec = {};
			switch (Dec.ID)
			{
			case CMonster::MONSTER_ID::CREEPER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Creeper"), &Dec);
				break;
			case CMonster::MONSTER_ID::ZOMBIE:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Zombie"), &Dec);
				break;
			case CMonster::MONSTER_ID::SKELETON:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton"), &Dec);
				break;
			case CMonster::MONSTER_ID::SKELETON_HORSE:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Skeleton_Horse"), &Dec);
				break;
			case CMonster::MONSTER_ID::GOLEM:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Golem"), &Dec);
				break;
			case CMonster::MONSTER_ID::REDSTONEBOSS:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_RedStoneBoss"), &Dec);
				break;
			case CMonster::MONSTER_ID::FIONA:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Fiona"), &Dec);
				break;
			case CMonster::MONSTER_ID::FORKLIFT:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_ForkLift"), &Dec);
				break;
			case CMonster::MONSTER_ID::VINDICATOR:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Vindicator"), &Dec);
				break;
			case CMonster::MONSTER_ID::ARCHILLAHER:
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Archillager"), &Dec);
				break;
			case CMonster::MONSTER_ID::NPC:
				eDec.Pos = Dec.Pos;
				eDec.RotationAngle = Dec.RotationAngle;
				eDec.Scale = Dec.Scale;
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Frendly"), TEXT("Prototype_GameObject_NPC"), &eDec);
				break;
			case CMonster::MONSTER_ID::ITEM_BOX:
				eDec.Pos = Dec.Pos;
				eDec.RotationAngle = Dec.RotationAngle;
				eDec.Scale = Dec.Scale;
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Frendly"), TEXT("Prototype_GameObject_ItemBox"), &eDec);
				break;
			case CMonster::MONSTER_ID::DOOR:
				eDec.Pos = Dec.Pos;
				eDec.RotationAngle = Dec.RotationAngle;
				eDec.Scale = Dec.Scale;
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Door"), TEXT("Prototype_GameObject_Door"), &eDec);
				break;
			case CMonster::MONSTER_ID::CHAIR:
				eDec.Pos = Dec.Pos;
				eDec.RotationAngle = Dec.RotationAngle;
				eDec.Scale = Dec.Scale;
				GetGI->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Frendly"), TEXT("Prototype_GameObject_Chair"), &eDec);
				break;
			default:
				break;
			}
		}

		inFile.close();
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Map(const wstring& strLayerTag)
{
	string finalPath = "../Bin/BinaryFile/SaveMap.dat";
	ifstream inFile(finalPath, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		return E_FAIL;
	}
	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		CMap::MAP_DEC dec = {};
		inFile.read((char*)&dec, sizeof(CMap::MAP_DEC));
		if (FAILED(GetGI->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MapTest"), &dec)))
			return E_FAIL;

	}
	inFile.close();

	return S_OK;

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_MapTest"))))
	//	return E_FAIL;

	//return S_OK;
}

HRESULT CLevel_GamePlay::Camera_Setting()
{
	CGameObject* Camera = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera");
	CGameObject* player = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player");
	CGameObject* mouse = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Mouse"), "Mouse");
	static_cast<CFreeCamera*>(Camera)->Set_Target(player);
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);

	m_pGameInstance->Add_Light(LightDesc);

	ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	LightDesc.vPosition = _float4(86.24f, 24.f, -104.74f, 1.f);
	LightDesc.fRange = 20.f;
	LightDesc.vDiffuse = _float4(1.f, 0.65f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;
	
	m_pGameInstance->Add_Light(LightDesc);
	//
	//ZeroMemory(&LightDesc, sizeof(LIGHT_DESC));
	//LightDesc.eType = LIGHT_DESC::TYPE_POINT;
	//LightDesc.vPosition = _float4(40.f, 5.f, 20.f, 1.f);
	//LightDesc.fRange = 20.f;
	//LightDesc.vDiffuse = _float4(0.0f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = _float4(0.1f, 0.4f, 0.1f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;
	//
	//m_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	/*ShowCursor(true);*/
	CollisionMgr::DestroyInstance();
	CInventory::GetInstance()->Free();
	CInventory::DestroyInstance();
	CUIManager::GetInstance()->Free();
	CUIManager::DestroyInstance();
	CParticleManager::DestroyInstance();
	__super::Free();


}
