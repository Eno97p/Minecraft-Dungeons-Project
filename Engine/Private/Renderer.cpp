#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"

#include "Shader.h"
#include "BlendObject.h"
#include "VIBuffer_Rect.h"

_uint		g_iSizeX = 8192;
_uint		g_iSizeY = 4608;

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderer::Initialize()
{
	D3D11_VIEWPORT			ViewportDesc{};
	_uint					iNumViewports = 1;

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* Target_Diffuse */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.0f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	/* Target_Normal */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Depth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_LightDepth */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_LightDepth"), g_iSizeX, g_iSizeY, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* Target_Shade */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	/* Target_Specular */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



	/* MRT_GameObjects */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;


	/* MRT_ShadowObject */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_ShadowObjects"), TEXT("Target_LightDepth"))))
		return E_FAIL;


	/* MRT_LightAcc */
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;


#pragma region BLOOM

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Effect"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Effect"), TEXT("Target_Effect"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_X"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_X"), TEXT("Target_Blur_X"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Blur_Y"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur_Y"), TEXT("Target_Blur_Y"))))
		return E_FAIL;




	//if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Brightness"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Brightness"), TEXT("Target_Brightness"))))
	//	return E_FAIL;




	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Result"), ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.0f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Result"), TEXT("Target_Result"))))
		return E_FAIL;


#pragma endregion BLOOM




	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	/* 화면을 꽉 채워주기 위한 월드변환행렬. */
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	if (nullptr == m_pDevice)
		return E_FAIL;
	ID3D11Texture2D* pDepthStencilTexture = nullptr;
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = g_iSizeX;
	TextureDesc.Height = g_iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pLightDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);


#ifdef _DEBUG
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Diffuse"), 100.0f, 100.0f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Normal"), 100.0f, 300.0f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Depth"), 100.0f, 500.0f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Shade"), 300.0f, 100.0f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Specular"), 300.0f, 300.0f, 200.f, 200.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_LightDepth"), ViewportDesc.Width - 150.0f, 450.0f, 300.f, 300.f)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Result"), 1180.f, 100.f, 200.f, 200.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Effect"), 1230.f, 200.f, 100.f, 100.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Blur_X"), 1230.f, 250.f, 100.f, 100.f)))
	//	return E_FAIL;
	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Blur_Y"), 1230.f, 300.f, 100.f, 100.f)))
	//	return E_FAIL;



	//if (FAILED(m_pGameInstance->Ready_RTDebug(TEXT("Target_Brightness"), 1230.f, 600.f, 100.f, 100.f)))
	//	return E_FAIL;



#endif

	return S_OK;
}

HRESULT CRenderer::Add_RenderObject(RENDERGROUP eRenderGroup, CGameObject* pRenderObject)
{
	m_RenderGroup[eRenderGroup].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

void CRenderer::Clear()
{
	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}
}

void CRenderer::Draw()
{
	Render_Priority();
	Render_ShadowObjects();
	Render_NonBlend();//4
	Render_LightAcc();//3
	Render_NonLight();

	Render_DeferredResult();//1

	Render_Bloom();
	Render_FinalBlend();

	Render_Blend();
	Render_UI();

#ifdef _DEBUG
	Render_Debug();
#endif
}
void CRenderer::Set_Position(_vector vPos)
{
	vPosition = vPos;
}
#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pComponent)
{
	m_DebugComponents.push_back(pComponent);

	Safe_AddRef(pComponent);

	return S_OK;
}
#endif
void CRenderer::Render_Priority()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Result"),nullptr)))
		return;

	for (auto& pGameObject : m_RenderGroup[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_PRIORITY].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	

}

void CRenderer::Render_ShadowObjects()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_ShadowObjects"), m_pLightDepthStencilView)))
		return;

	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)g_iSizeX;
	ViewPortDesc.Height = (_float)g_iSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);


	for (auto& pGameObject : m_RenderGroup[RENDER_SHADOWOBJ])
	{
		if (nullptr != pGameObject)
			pGameObject->Render_LightDepth();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_SHADOWOBJ].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)1280.0f;
	ViewPortDesc.Height = (_float)720.0f;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);
}

void  CRenderer::Render_NonBlend()
{
	/* Diffuse + Normal */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"), nullptr)))
		return;

	for (auto& pGameObject : m_RenderGroup[RENDER_NONBLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_NONBLEND].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

}

void CRenderer::Render_LightAcc()
{
	/* Shade */
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"), nullptr)))
		return;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;



	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return;

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
		return;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;


	

	m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

	if (FAILED(m_pGameInstance->End_MRT()))
		return;
}

void CRenderer::Render_DeferredResult()
{
	m_pGameInstance->Begin_MRT(TEXT("MRT_Result"), false);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return;


	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW))))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_PROJ))))
		return;
	_float4x4		ViewMatrix, ProjMatrix;

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(68.f, 150.f, -126.f, 0.f), XMVectorSet(83.f, 24.f, -36.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iSizeX / g_iSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShader->Bind_Matrix("g_LightViewMatrix", &ViewMatrix)))
		return;
	if (FAILED(m_pShader->Bind_Matrix("g_LightProjMatrix", &ProjMatrix)))
		return;


	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_LightDepth"), m_pShader, "g_LightDepthTexture")))
		return;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
		return;
	
	m_pShader->Begin(3);

	m_pVIBuffer->Bind_Buffers();

	m_pVIBuffer->Render();

	m_pGameInstance->End_MRT();
}

void  CRenderer::Render_NonLight()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Effect"), nullptr)))
		return;

	for (auto& pGameObject : m_RenderGroup[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_NONLIGHT].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return;

	//if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Brightness"))))
	//	return;
	//
	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture")))
	//	return;
	//
	//m_pVIBuffer->Bind_Buffers();
	//m_pShader->Begin(7);
	//m_pVIBuffer->Render();
	//
	//
	//if (FAILED(m_pGameInstance->End_MRT()))
	//	return;



}

void  CRenderer::Render_Blend()
{
	if (m_RenderGroup[RENDER_BLEND].size() < 1)
		return;

	m_RenderGroup[RENDER_BLEND].sort([](CGameObject* pSour, CGameObject* pDest)->_bool
		{
			return dynamic_cast<CBlendObject*>(pSour)->Get_CamDistance() > dynamic_cast<CBlendObject*>(pDest)->Get_CamDistance();
		});

	for (auto& pGameObject : m_RenderGroup[RENDER_BLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_BLEND].clear();
}

void  CRenderer::Render_UI()
{
	for (auto& pGameObject : m_RenderGroup[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderGroup[RENDER_UI].clear();
}

HRESULT CRenderer::Render_Bloom()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_X"), nullptr)))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Brightness"), m_pShader, "g_EffectTexture")))
	//	return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pShader->Begin(4);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Blur_Y"), nullptr)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Blur_X"), m_pShader, "g_EffectTexture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pShader->Begin(5);
	m_pVIBuffer->Render();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	return S_OK;

}

HRESULT CRenderer::Render_FinalBlend()
{
	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Blur_Y"), m_pShader, "g_BlurTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Effect"), m_pShader, "g_EffectTexture")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Bind_RenderTargetSRV(TEXT("Target_Result"), m_pShader, "g_ResultTexture")))
		return E_FAIL;

	m_pVIBuffer->Bind_Buffers();
	m_pShader->Begin(6);
	m_pVIBuffer->Render();


	return S_OK;
}

#ifdef _DEBUG

void CRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		pDebugCom->Render();
		Safe_Release(pDebugCom);
	}
	m_DebugComponents.clear();

	m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
	m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

	m_pGameInstance->Render_RTDebug(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_ShadowObjects"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_Result"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_Blur_X"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_Blur_Y"), m_pShader, m_pVIBuffer);
	m_pGameInstance->Render_RTDebug(TEXT("MRT_Brightness"), m_pShader, m_pVIBuffer);

}

#endif

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderer::Free()
{
	Safe_Release(m_pGameInstance);

	for (size_t i = 0; i < RENDER_END; i++)
	{
		for (auto& pGameObject : m_RenderGroup[i])
			Safe_Release(pGameObject);

		m_RenderGroup[i].clear();
	}

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pLightDepthStencilView);
}
