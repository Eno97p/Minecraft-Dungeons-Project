#pragma once

#include "Base.h"

/* 원형객체들을 보관한다. */
/* 특정 원형객체를 검색하여 복제하고, 복제한 객체(사본)를 레벨(동적할당)별로 그룹(Layer)지어 보관한다. */
/* 보관하고 있는 사본 객체들의 반복적인 Tick함수들을 호출해준다. */
BEGIN(Engine)
class CGameObject;
END

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:	
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, void* pArg);
	HRESULT Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, class CGameObject** pp, void* pArg);
	class CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	void Priority_Tick(_float fTimeDelta);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Clear(_uint iLevelIndex);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);
	void Delete_Object(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);
	void Delete_All(_uint iLevelIndex, const wstring& strLayerTag);
	//for Event Manager
	void Add_Object(_uint iLevelIndex, const wstring& strLayerTag, class CGameObject* pGameObject);

	HRESULT Delete_Event(vector<class CGameObject*> DeadObjs);

	CGameObject* Find_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag);
	CGameObject* Find_Object_By_ID(_uint iLevelIndex, const wstring& strLayerTag, const char* _ID);
	list<class CGameObject*> Get_Layer_Objects(_uint iLevelIndex, const wstring& strLayerTag);

	class CGameObject* Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex);

private:
	map<const wstring, class CGameObject*>				m_Prototypes;
	map<const wstring, class CLayer*>*					m_pLayers = { nullptr };

	_uint												m_iNumLevels = { 0 };
	_uint												m_iNumLayers = { 0 };
private:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	


public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END