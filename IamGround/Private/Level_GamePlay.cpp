
#include "..\Public\Level_GamePlay.h"
#include "GameInstance.h"

#include "FreeCamera.h"
#include "ImguiMgr.h"
#include "GameObject.h"
#include "Level_Loading.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	return S_OK;
}



void CLevel_GamePlay::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이레벨임"));
#endif
	
	if (GetKeyState('P') & 0x8000)
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_EFFECT))))
			return;
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

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_FreeCamera"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 0.5f);

	m_pGameInstance->Add_Light(LightDesc);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{

	//CGameObject* dfasdf; 

	//if (FAILED(m_pGameInstance->Add_CloneObjectZ(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"),&dfasdf, nullptr)))
	//	return E_FAIL;
	//CImguiMgr::GetInstance()->Add_TerrainList(dfasdf);
	

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring& strLayerTag)
{

	//if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"))))
	//	return E_FAIL;

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
	__super::Free();


}
