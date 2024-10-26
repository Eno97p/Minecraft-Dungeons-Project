#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
END

BEGIN(Client)
class CUIManager :   public CBase
{
	DECLARE_SINGLETON(CUIManager)
public:
	enum UISORTTYPE { FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, SEVENTH, EIGHT, NINETH, SORT_END};
public:
	CUIManager();
	virtual ~CUIManager() = default;
public:
	HRESULT Initialize();

	void Tick(_float fTimeDelta);
	HRESULT Add_RenderUI(CGameObject* ui, UISORTTYPE type);

private:
	void Clear();
private:
	CGameInstance* m_pGameInstance = nullptr;
	vector<CGameObject*>  m_VecUI[SORT_END];
public:
	virtual void Free() override;
};

END