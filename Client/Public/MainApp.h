#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void Tick(float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	CGameInstance*			m_pGameInstance = { nullptr };


private:
	_tchar					m_szFPS[MAX_PATH] = TEXT("");
	_float					m_fTimeAcc = {};
	_uint					m_iNumRender = {};



public:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();


public:
	static CMainApp* Create();
	virtual void Free() override;
};

END