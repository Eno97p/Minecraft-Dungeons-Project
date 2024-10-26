
#include "Level_Effect.h"
#include "GameInstance.h"


#include "GameObject.h"
#include "Map.h"
#include "QuarterCamera.h"
#include "ParticleMesh.h"


CLevel_Effect::CLevel_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Effect::Initialize()
{
	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;
	return S_OK;
}



void CLevel_Effect::Tick(_float fTimeDelta)
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("이펙트레벨임"));
#endif
	
}

HRESULT CLevel_Effect::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CQuarterCamera::FREE_CAMERA_DESC		CameraDesc{};

	CameraDesc.fSensor = 0.1f;
	CameraDesc.vEye = _float4(0.0, 10.0f, -20.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.0f, 0.0f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = g_iWinSizeX / (_float)g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 3000.f;
	CameraDesc.fSpeedPerSec = 20.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(m_pGameInstance->Add_CloneObject(LEVEL_EFFECT, strLayerTag, TEXT("Prototype_GameObject_QuarterCamera"), &CameraDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Effect::Ready_Lights()
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

HRESULT CLevel_Effect::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	CMap::MAP_DEC dec{};
	dec.Pos = _float4(0.f, 0.f, 0.f, 1.f);
	dec.Scale = _float3(1.f, 1.f, 1.f);

	CGameObject* Flat = nullptr;

	if(FAILED(m_pGameInstance->Add_CloneObjectZ(LEVEL_EFFECT, strLayerTag, TEXT("Prototype_GameObject_FlatMap"),&Flat, &dec)))
		return E_FAIL;

	static_cast<CQuarterCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_EFFECT, TEXT("Layer_Camera"), "Quarter_Camera"))->Set_Target(Flat);
	
	//CVIBuffer_Instance::INSTANCE_DESC InstanceDesc{};
	//InstanceDesc.iNumInstance = 100;
	//InstanceDesc.vOffsetPos = _float3(0.0f, -3.f, 0.0f);
	//InstanceDesc.vPivotPos = _float3(0.0f, 0.1f, 0.0f);
	//InstanceDesc.vRange = _float3(3.0f, 5.0f, 3.0f);
	//InstanceDesc.vSize = _float2(0.01f, 0.3f);
	//InstanceDesc.vSpeed = _float2(1.f, 7.f);
	//InstanceDesc.vLifeTime = _float2(1.f, 3.f);
	//InstanceDesc.vGravity = _float2(1.f, 5.f);
	//InstanceDesc.isLoop = true;





	return S_OK;
}


CLevel_Effect * CLevel_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Effect*		pInstance = new CLevel_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLevel_Effect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Effect::Free()
{
	__super::Free();


}
