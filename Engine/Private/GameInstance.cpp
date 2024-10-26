#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "GameObject.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Renderer.h"
#include "Picking.h"
#include "Calculator.h"
#include "Light_Manager.h"
#include "EventMgr.h"
#include "Font_Manager.h"
#include "Target_Manager.h"
#include "SoundMgr.h"
#include "Frustum.h"
IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}


HRESULT CGameInstance::Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC & EngineDesc, ID3D11Device ** ppDevice, ID3D11DeviceContext ** ppContext)
{
	/* ���� ������Ʈ�� ��Ŭ���̾�Ʈ���� ����ϱ� ���� �ʼ������� �ؾ��� ���� �ʱ�ȭ�۾��� �����Ѵ�. */

	/* �׷��� ����̽��� �ʱ�ȭ�ϳ�. */
	m_pGraphic_Device = CGraphic_Device::Create(EngineDesc, ppDevice, ppContext);
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pTimer_Manager = CTimer_Manager::Create();
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	/* ��ǲ ����̽��� �ʱ�ȭ�ϳ�. */
	m_pInput_Device = CInput_Device::Create(hInst, EngineDesc.hWnd);
	if (nullptr == m_pInput_Device)
		return E_FAIL;
	
	/* ����  ����̽��� �ʱ�ȭ�ϳ�. */
	

	/* ���� �Ŵ����� �غ� ����. */
	m_pLevel_Manager = CLevel_Manager::Create();
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	/* ������Ʈ, ������Ʈ �Ŵ����� ����ϱ� ���� �غ� �Ѵ�.*/
	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels);
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	m_pTarget_Manager = CTarget_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;
	/* ������ ����. */
	m_pRenderer = CRenderer::Create(*ppDevice, *ppContext);
	if (nullptr == m_pRenderer)
		return E_FAIL;

	/* ���������� ����. */
	m_pPipeLine = CPipeLine::Create();
	if (nullptr == m_pPipeLine)
		return E_FAIL;

	//Calculator ����
	m_pCalculator = CCalculator::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pCalculator)
		return E_FAIL;

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, EngineDesc.hWnd);
	if (nullptr == m_pPicking)
		return E_FAIL;

	m_pLight_Manager = CLight_Manager::Create();
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	m_pEvent_Manager = CEventMgr::Create(m_pObject_Manager);
	if (nullptr == m_pEvent_Manager)
		return E_FAIL;

	m_pSound_Manager = CSoundMgr::Create();
	if (nullptr == m_pSound_Manager)
		return E_FAIL;

	m_pFrustum = CFrustum::Create();
	if (nullptr == m_pFrustum)
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (nullptr == m_pLevel_Manager)
		return;

	m_pInput_Device->Update_InputDev();

	m_pObject_Manager->Priority_Tick(fTimeDelta);

	m_pObject_Manager->Tick(fTimeDelta);	

	

	m_pPipeLine->Tick();
//#ifdef _DEBUG
	m_pPicking->Update();
//#endif
	m_pFrustum->Update();

	m_pCalculator->Store_MouseRay(m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ),
		m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW));

	m_pObject_Manager->Late_Tick(fTimeDelta);

	m_pLevel_Manager->Tick(fTimeDelta);
}

HRESULT CGameInstance::Draw()
{
	/*if (FAILED(Clear_BackBuffer_View(vClearColor)))
		return E_FAIL;
	if (FAILED(Clear_DepthStencil_View()))
		return E_FAIL;*/

	m_pRenderer->Draw();	

	return S_OK;
}

void CGameInstance::Clear_Resources(_uint iLevelIndex)
{
	m_pRenderer->Clear();
	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	return m_pGraphic_Device->Present();
}


_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	return m_pInput_Device->Get_DIKeyState(byKeyID);	
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->Get_DIMouseState(eMouse);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
	return m_pInput_Device->Get_DIMouseMove(eMouseState);
}

MOUSEWHEELSTATE CGameInstance::CheckMouseWheel(void)
{
	return m_pInput_Device->CheckMouseWheel();
}

_bool CGameInstance::MouseButtonUp(MOUSEKEYSTATE eMouse)
{
	return m_pInput_Device->MouseButtonUp(eMouse);
}

_bool CGameInstance::IsKeyReleased(_ubyte _Key)
{
	return m_pInput_Device->IsKeyReleased(_Key);
}

_bool CGameInstance::isKeyUP(_int _iKey)
{
	return m_pInput_Device->isKeyUP(_iKey);
}

_bool CGameInstance::isKeyDown(_int _iKey)
{
	return m_pInput_Device->isKeyDown(_iKey);
}

_float CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	return m_pTimer_Manager->Get_TimeDelta(pTimerTag);	
}

void CGameInstance::Update_TimeDelta(const _tchar * pTimerTag)
{
	m_pTimer_Manager->Update_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	return m_pTimer_Manager->Ready_Timer(pTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

_uint CGameInstance::Get_CurrentLevel()
{
	return m_pLevel_Manager->Get_CurrentLevel();
}

HRESULT CGameInstance::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag,  void * pArg )
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_CloneObjectZ(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, CGameObject** ppGameobject, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_CloneObject(iLevelIndex, strLayerTag, strPrototypeTag, ppGameobject, pArg);
}

CGameObject* CGameInstance::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

void CGameInstance::Delete_Object(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	m_pObject_Manager->Delete_Object(iLevelIndex, strLayerTag, pGameObject);
}

void CGameInstance::Delete_All(_uint iLevelIndex, const wstring& strLayerTag)
{
	m_pObject_Manager->Delete_All(iLevelIndex,strLayerTag);
}

CGameObject* CGameInstance::Find_Object_By_ID(_uint iLevelIndex, const wstring& strLayerTag, const char* _ID)
{
	return m_pObject_Manager->Find_Object_By_ID(iLevelIndex, strLayerTag, _ID);
}

list<class CGameObject*> CGameInstance::Get_Layer_Objects(_uint iLevelIndex, const wstring& strLayerTag)
{
	return m_pObject_Manager->Get_Layer_Objects(iLevelIndex, strLayerTag);
}

CGameObject* CGameInstance::Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Object(iLevelIndex, strLayerTag, iIndex);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring & strPrototypeTag, CComponent * pPrototype)
{
	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring & strPrototypeTag, void * pArg)
{
	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, CGameObject * pRenderObject)
{
	if (nullptr == m_pRenderer)
		return E_FAIL;

	return m_pRenderer->Add_RenderObject(eRenderGroup, pRenderObject);	
}
#ifdef _DEBUG
HRESULT CGameInstance::Add_DebugComponent(CComponent* pComponent)
{
	return m_pRenderer->Add_DebugComponent(pComponent);
}
#endif
const _float4x4 * CGameInstance::Get_Transform_float4x4(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix(eState);
}

const _float4x4 * CGameInstance::Get_Transform_float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_float4x4_Inverse(eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState)
{
	return m_pPipeLine->Get_Transform_Matrix_Inverse(eState);
}

const _float4 * CGameInstance::Get_CamPosition_float4()
{
	return m_pPipeLine->Get_CamPosition_float4();
}

_vector CGameInstance::Get_CamPosition()
{
	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

//_vector CGameInstance::Mouse_Picking(CVIBuffer_Terrain* pTerrainBufferCom, CTransform* _transform)
//{
//	return m_pCalculator->Mouse_Picking(pTerrainBufferCom, _transform, m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_PROJ),
//		m_pPipeLine->Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE::D3DTS_VIEW));
//}



_vector CGameInstance::Get_RayPos()
{
	if(m_pCalculator == nullptr)
		return _vector();
	else
		return m_pCalculator->Get_RayPos();
}

_vector CGameInstance::Get_RayDir()
{
	if (m_pCalculator == nullptr)
		return _vector();
	else
		return m_pCalculator->Get_RayDir();
}

void CGameInstance::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, CTransform* pTransform)
{
	m_pCalculator->Compute_LocalRayInfo(pRayDir, pRayPos, pTransform);
}

void CGameInstance::Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse)
{
	m_pCalculator->Compute_LocalRayInfo(pRayDir, pRayPos, WorldInverse);
}

POINT CGameInstance::Get_Mouse_Point()
{
	return m_pCalculator->Get_Mouse_Point();
}

_bool CGameInstance::Get_PickPos(_float4* pPickPos)
{
	return m_pPicking->Get_PickPos(pPickPos);
}

ID3D11Texture2D* CGameInstance::Get_DepthTexture()
{
	return m_pPicking->Get_DepthTexture();
}


const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iIndex) const
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(const LIGHT_DESC& LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(LightDesc);
}

HRESULT CGameInstance::Render_Lights(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Render(pShader, pVIBuffer);
}

HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFontFilePath)
{
	return m_pFont_Manager->Add_Font(strFontTag, strFontFilePath);
}

HRESULT CGameInstance::Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor)
{
	return m_pFont_Manager->Render_Font(strFontTag, strText, vPosition, vColor);
}

HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
	return m_pTarget_Manager->Add_RenderTarget(strTargetTag, iSizeX, iSizeY, ePixelFormat, vClearColor);
}

HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	return m_pTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, _bool isClear)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, isClear);
}

HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView)
{
	return m_pTarget_Manager->Begin_MRT(strMRTTag, pDSView);
}

HRESULT CGameInstance::End_MRT()
{
	return m_pTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTargetSRV(const wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
	return m_pTarget_Manager->Bind_RenderTargetSRV(strTargetTag, pShader, pConstantName);
}
HRESULT CGameInstance::Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc)
{
	return m_pTarget_Manager->Copy_Resource(strTargetTag, pDesc);
}
#ifdef _DEBUG
HRESULT CGameInstance::Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	return m_pTarget_Manager->Ready_Debug(strTargetTag, fX, fY, fSizeX, fSizeY);
}

HRESULT CGameInstance::Render_RTDebug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	return m_pTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif
HRESULT CGameInstance::EventUpdate()
{
	return m_pEvent_Manager->EventUpdate();
}

void CGameInstance::CreateObject(_uint Level, const wchar_t* Layer , class CGameObject* pObj)
{
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::CREATE_OBJECT;
	evn.lParam = (DWORD_PTR)Level;
	evn.wParam = (DWORD_PTR)Layer;
	evn.pParam = (DWORD_PTR)pObj;

	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::Erase(CGameObject* _pObj)
{
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::DELETE_OBJECT;
	evn.lParam = (DWORD_PTR)_pObj;
	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::Clear_Vector_Array(vector<CGameObject*>* pVector)
{
	tEvent evn = {};
	evn.eEven = eEVENT_TYPE::CLEAR_UI;
	evn.lParam = (DWORD_PTR)pVector;
	m_pEvent_Manager->AddEvent(evn);
}

void CGameInstance::PlaySound_Z(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	m_pSound_Manager->PlaySound_Z(pSoundKey, eID, fVolume);
}

void CGameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
	m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}

void CGameInstance::StopAll()
{
	m_pSound_Manager->StopAll();
}

void CGameInstance::StopSound(CHANNELID eID)
{
	m_pSound_Manager->StopSound(eID);
}

void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pSound_Manager->SetChannelVolume(eID, fVolume);
}

void CGameInstance::PlaySubBGM(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->PlaySubBGM(pSoundKey, fVolume);
}

void CGameInstance::PlaySubBGM2(const TCHAR* pSoundKey, _float fVolume)
{
	m_pSound_Manager->PlaySubBGM2(pSoundKey, fVolume);
}

_bool CGameInstance::isSoundPlaying(CHANNELID eID)
{
	return m_pSound_Manager->isSoundPlaying(eID);
}

void CGameInstance::Transform_ToLocalSpace(_fmatrix WorldMatrixInv)
{
	return m_pFrustum->Transform_ToLocalSpace(WorldMatrixInv);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_WorldFrustum(vPosition, fRange);
}

_bool CGameInstance::isIn_LocalFrustum(_fvector vPosition, _float fRange)
{
	return m_pFrustum->isIn_LocalFrustum(vPosition, fRange);
}

void CGameInstance::Release_Engine()
{	
	CGameInstance::GetInstance()->Free();

	DestroyInstance();	
}

void CGameInstance::Free()
{	
	Safe_Release(m_pFrustum);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pCalculator);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pPicking);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pEvent_Manager);
	Safe_Release(m_pSound_Manager);
}

