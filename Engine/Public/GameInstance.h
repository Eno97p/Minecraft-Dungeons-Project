#pragma once

#include "Renderer.h"
#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	HRESULT Initialize_Engine(HINSTANCE hInst, _uint iNumLevels, const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice, _Out_ ID3D11DeviceContext** ppContext);
	void Tick_Engine(_float fTimeDelta);
	HRESULT Draw();
	void Clear_Resources(_uint iLevelIndex);

public: /* For.Graphic_Device */	
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();


public: /* For.Input_Device */
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
	MOUSEWHEELSTATE CheckMouseWheel(void);
	_bool MouseButtonUp(MOUSEKEYSTATE eMouse);
	_bool IsKeyReleased(_ubyte _Key);
	_bool isKeyUP(_int _iKey);
	_bool isKeyDown(_int _iKey);
public: /* For.Timer_Manager */
	_float		Get_TimeDelta(const _tchar* pTimerTag);
	void		Update_TimeDelta(const _tchar* pTimerTag);
	HRESULT		Ready_Timer(const _tchar* pTimerTag);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	_uint Get_CurrentLevel();

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex,
		const wstring& strLayerTag,
		const wstring& strPrototypeTag,
		void* pArg = nullptr
		);
	HRESULT Add_CloneObjectZ(_uint iLevelIndex,
		const wstring& strLayerTag,
		const wstring& strPrototypeTag,
		class CGameObject** ppGameobject,
		void* pArg = nullptr
	);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void Delete_Object(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);
	void Delete_All(_uint iLevelIndex, const wstring& strLayerTag);
	class CGameObject* Find_Object_By_ID(_uint iLevelIndex, const wstring& strLayerTag, const char* _ID);
	list<class CGameObject*> Get_Layer_Objects(_uint iLevelIndex, const wstring& strLayerTag);
	class CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex = 0);


public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg = nullptr);

public: /* For.Renderer */
	HRESULT Add_RenderObject(CRenderer::RENDERGROUP eRenderGroup, class CGameObject* pRenderObject);
#ifdef _DEBUG
public:
	HRESULT Add_DebugComponent(class CComponent* pComponent);
#endif


public: /* For.PipeLine */
	const _float4x4* Get_Transform_float4x4(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4x4* Get_Transform_float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE eState);
	const _float4* Get_CamPosition_float4();
	_vector Get_CamPosition();
	void Set_Transform(CPipeLine::D3DTRANSFORMSTATE eState, _fmatrix TransformMatrix);

public: /*for Calculator*/
	_vector Get_RayPos();
	_vector Get_RayDir();
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, class CTransform* pTransform);
	void Compute_LocalRayInfo(_float3* pRayDir, _float3* pRayPos, XMMATRIX WorldInverse);
	POINT Get_Mouse_Point();

public: /* For.Picking */
	_bool Get_PickPos(_float4* pPickPos);
	ID3D11Texture2D* Get_DepthTexture();

public: /* For.Light_Manager */
	const LIGHT_DESC* Get_LightDesc(_uint iIndex) const;
	HRESULT Add_Light(const LIGHT_DESC& LightDesc);
	HRESULT Render_Lights(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

public: /* For.Font_Manager */
	HRESULT Add_Font(const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render_Font(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _fvector vColor);

public: /* For.Target_Manager */
	HRESULT Add_RenderTarget(const wstring& strTargetTag, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vClearColor);
	HRESULT Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT Begin_MRT(const wstring& strMRTTag , _bool isClear = true);
	HRESULT Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDSView = nullptr);
	HRESULT End_MRT();
	HRESULT Bind_RenderTargetSRV(const wstring& strTargetTag, class CShader* pShader, const _char* pConstantName);
	HRESULT Copy_Resource(const wstring& strTargetTag, ID3D11Texture2D* pDesc);
#ifdef _DEBUG
public:
	HRESULT Ready_RTDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render_RTDebug(const wstring& strMRTTag, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
public:	//for Event_Manager
	HRESULT EventUpdate();
	void CreateObject(_uint Level, const wchar_t* Layer, class CGameObject* pObj);
	void Erase(CGameObject* _pObj);
	void Clear_Vector_Array(vector<CGameObject*>* pVector);

public: //for Sound_Manager
	void PlaySound_Z(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopAll();
	void StopSound(CHANNELID eID);
	void SetChannelVolume(CHANNELID eID, float fVolume);
	void PlaySubBGM(const TCHAR* pSoundKey, _float fVolume);
	void PlaySubBGM2(const TCHAR* pSoundKey, _float fVolume);
	_bool isSoundPlaying(CHANNELID eID);

public:
	void Transform_ToLocalSpace(_fmatrix WorldMatrixInv);
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);
	_bool isIn_LocalFrustum(_fvector vPosition, _float fRange = 0.f);
		
private:
	class CGraphic_Device*			m_pGraphic_Device = { nullptr };
	class CInput_Device*			m_pInput_Device = { nullptr };
	class CTimer_Manager*			m_pTimer_Manager = { nullptr };
	class CLevel_Manager*			m_pLevel_Manager = { nullptr };
	class CObject_Manager*			m_pObject_Manager = { nullptr };
	class CComponent_Manager*		m_pComponent_Manager = { nullptr };
	class CRenderer*				m_pRenderer = { nullptr };
	class CPipeLine*				m_pPipeLine = { nullptr };
	class CCalculator*				m_pCalculator = { nullptr };
	class CPicking*					m_pPicking = { nullptr };
	class CLight_Manager*			m_pLight_Manager = { nullptr };
	class CEventMgr*				m_pEvent_Manager = { nullptr };
	class CFont_Manager*			m_pFont_Manager = { nullptr };
	class CTarget_Manager*			m_pTarget_Manager = { nullptr };
	class CSoundMgr*				m_pSound_Manager = { nullptr };
	class CFrustum*					m_pFrustum = { nullptr };

public:	
	static void Release_Engine();
	virtual void Free() override;
};

END