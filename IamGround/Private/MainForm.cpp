
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "MainForm.h"
#include "GameInstance.h"
#include "Shader.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "ImguiMgr.h"


CMainForm::CMainForm()
	: m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);

}

HRESULT CMainForm::Initialize()
{
	ENGINE_DESC			EngineDesc{};
	
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowed = true;

	/* 엔진 초기화과정을 거친다. ( 그래픽디바이스 초기화과정 + 레벨매니져를 사용할 준비를 한다. ) */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	//if (FAILED(Ready_Testing_Navigation()))
	//	return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	CImguiMgr::GetInstance()->Initialize(m_pDevice, m_pContext);

	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();



	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	ImGui::StyleColorsDark();*/


	return S_OK;
}

void CMainForm::Tick(float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
	CImguiMgr::GetInstance()->Tick(fTimeDelta);
}

HRESULT CMainForm::Render()
{

	CImguiMgr::GetInstance()->Render();

	
	if (FAILED(m_pGameInstance->Present()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->EventUpdate()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainForm::Open_Level(LEVEL eLevelID)
{
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainForm::Ready_Prototype_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainForm::Ready_Prototype_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/LoadingTex/load (%d).png"), 71))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainForm::Ready_Testing_Navigation()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	_float3		vPoints[3] = {};

	string FilePath = "../Bin/BinaryFile/Navigation.dat";
	ofstream file(FilePath, ios::out | ios::binary);

	if (file.is_open())
	{
		_uint Numvertex = 4;
		file.write((char*)&Numvertex, sizeof(_uint));


		vPoints[0] = _float3(0.f, 0.f, 10.f);
		vPoints[1] = _float3(10.f, 0.f, 0.f);
		vPoints[2] = _float3(0.f, 0.f, 0.f);
		file.write((char*)&vPoints, sizeof(_float3) * 3);
		
		vPoints[0] = _float3(0.f, 0.f, 10.f);
		vPoints[1] = _float3(10.f, 0.f, 10.f);
		vPoints[2] = _float3(10.f, 0.f, 0.f);
		file.write((char*)&vPoints, sizeof(_float3) * 3);

		vPoints[0] = _float3(0.f, 0.f, 20.f);
		vPoints[1] = _float3(10.f, 0.f, 10.f);
		vPoints[2] = _float3(0.f, 0.f, 10.f);
		file.write((char*)&vPoints, sizeof(_float3) * 3);

		vPoints[0] = _float3(10.f, 0.f, 10.f);
		vPoints[1] = _float3(20.f, 0.f, 0.f);
		vPoints[2] = _float3(10.f, 0.f, 0.0f);
		file.write((char*)&vPoints, sizeof(_float3) * 3);

		file.close();
		MSG_BOX("Save Navigation Complete");
	}
	else
	{
		file.close();
		MSG_BOX("Failed_OpenFile");
		return E_FAIL;
	}
	



	return S_OK;
}

CMainForm* CMainForm::Create()
{
	CMainForm* pInstance = new CMainForm();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CMainForm");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainForm::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CImguiMgr::DestroyInstance();

	CGameInstance::Release_Engine();
}
