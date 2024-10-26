#pragma once

#include "Base.h"
BEGIN(Engine)
class CGameObject;
END

BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Delete_Object(class CGameObject* pGameObject);
	void Delete_All();
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	CGameObject* Find_Obj(CGameObject* pfind);
	list<class CGameObject*> Get_Layer_Objects() { return m_GameObjects; }
	CGameObject* Find_GameObject_From_ID(const char* _ID);
	class CGameObject* Get_Object(_uint iIndex);
	
private:
	list<class CGameObject*>		m_GameObjects;

public:
	static CLayer* Create();
	virtual void Free() override;
};

END