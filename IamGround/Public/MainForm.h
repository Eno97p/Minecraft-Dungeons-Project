#pragma once
#include "Base.h"
#include "Tool_Define.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(TOOL)
class CMainForm :
    public CBase
{
private:
	CMainForm();
	virtual ~CMainForm() = default;

public:
	HRESULT Initialize();
	void Tick(float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	CGameInstance* m_pGameInstance = { nullptr };

public:
	HRESULT Open_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_GameObject();
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Testing_Navigation();


public:
	static CMainForm* Create();
	virtual void Free() override;

};

END