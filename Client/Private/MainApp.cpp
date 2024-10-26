#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Mouse.h"
#include "Shader.h"
#include <thread>

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
}

HRESULT CMainApp::Initialize()
{

	ENGINE_DESC			EngineDesc{};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	/* 엔진 초기화과정을 거친다. ( 그래픽디바이스 초기화과정 + 레벨매니져를 사용할 준비를 한다. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;	

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Default"), TEXT("../Bin/Resources/Fonts/Mincraft.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Middle"), TEXT("../Bin/Resources/Fonts/Middle.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_Tiny"), TEXT("../Bin/Resources/Fonts/Hancom18.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Font_TinyEng"), TEXT("../Bin/Resources/Fonts/Tiny.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Nanum10"), TEXT("../Bin/Resources/Fonts/Nanum10.spritefont"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Font(TEXT("Nexon"), TEXT("../Bin/Resources/Fonts/140.spritefont"))))
		return E_FAIL;

	
	

	return S_OK;
}

void CMainApp::Tick(float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	//std::thread::id this_thread_id = std::this_thread::get_id();

	//// 스레드 ID 출력
	//std::cout << "현재 스레드 ID: " << this_thread_id << std::endl;

	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{

	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();
	

	m_pGameInstance->Draw();

	//m_pGameInstance->Render_Font(TEXT("Font_Default"), m_szFPS, _float2(0.f, 0.f), XMVectorSet(1.f, 1.f, 0.f, 1.f));

	m_pGameInstance->Present();


 	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;

	return S_OK;
}


HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{
	/* For.Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"), CMouse::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), 
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Logo/loadingGame.png"), 1))))
		return E_FAIL;

	//mouseTexture
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/MouseCursor/mouse%d.png"), 4))))
		return E_FAIL;



	return S_OK;
}




CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	/* 레퍼런스 카운트를 0으로만든다. */
	m_pGameInstance->Release_Engine();
	/*Safe_Release(m_pGameInstance);*/
}
