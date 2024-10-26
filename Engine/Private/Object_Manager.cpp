#include "..\Public\Object_Manager.h"
#include "GameObject.h"
#include "Layer.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new map<const wstring, class CLayer*>[iNumLevels];

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring & strLayerTag, const wstring & strPrototypeTag,  void * pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject*		pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*		pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	CLayer*				pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);	
		
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		++m_iNumLayers;
	}
	/* 내가 추가할려가하ㅡㄴㄴ 레이어가 있었다!!  */
	else
	{
		pLayer->Add_GameObject(pCloneObject);
	}


	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag, CGameObject** pp, void* pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	/* 내가 추가할려가하ㅡㄴㄴ 레이어가 있었다!!  */
	else
	{
		pLayer->Add_GameObject(pCloneObject);
	}
	*pp = pCloneObject;

	return S_OK;
}

CGameObject* CObject_Manager::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	/* 내가 복제해야할 우언형을 검색하낟. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pCloneObject = pPrototype->Clone(pArg);
	if (nullptr == pCloneObject)
		return nullptr;

	return pCloneObject;
}

void CObject_Manager::Priority_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Priority_Tick(fTimeDelta);
	}
}

void CObject_Manager::Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Pair.second->Tick(fTimeDelta);
	}
}

void CObject_Manager::Late_Tick(_float fTimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Pair.second->Late_Tick(fTimeDelta);
		}
	}	
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return;

	for (auto& Pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_Object(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strPrototypeTag)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
	{
		MSG_BOX("Failed To Find Prototype");
		return nullptr;
	}

	//CGameObject* pCloneObject = pPrototype->Clone(pArg);
	//if (nullptr == pCloneObject)
	//	return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		MSG_BOX("Failed To Find Layer");
		return nullptr;

	}
	/* 내가 추가할려가하ㅡㄴㄴ 레이어가 있었다!!  */
	return nullptr;
}

CGameObject* CObject_Manager::Find_Object_By_ID(_uint iLevelIndex, const wstring& strLayerTag, const char* _ID)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 아직 추가할려고하는 레이어가 없었따?!! */
	/* 레이어를 생성해서 추가하믄되겄다. */
	if (nullptr == pLayer)
	{
		MSG_BOX("Failed To Find Layer");
		return nullptr;
	}
	else
	{
		if(pLayer->Find_GameObject_From_ID(_ID) != nullptr)
			return pLayer->Find_GameObject_From_ID(_ID);
		else
		{
			MSG_BOX("Failed To Find ID");
			return nullptr;
		}
	}
}

list<class CGameObject*> CObject_Manager::Get_Layer_Objects(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		MSG_BOX("Failed To Find Layer");
		return list<class CGameObject*>();
	}
	else
	{
		return pLayer->Get_Layer_Objects();
	}
}

CGameObject* CObject_Manager::Get_Object(_uint iLevelIndex, const wstring& strLayerTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;
	return pLayer->Get_Object(iIndex);
}



CGameObject * CObject_Manager::Find_Prototype(const wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring & strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Delete_Object(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	pLayer->Delete_Object(pGameObject);
}

void CObject_Manager::Delete_All(_uint iLevelIndex, const wstring& strLayerTag)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (pLayer == nullptr)
		return;
	pLayer->Delete_All();
}

void CObject_Manager::Add_Object(_uint iLevelIndex, const wstring& strLayerTag, CGameObject* pGameObject)
{
	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
		++m_iNumLayers;
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}
}

HRESULT CObject_Manager::Delete_Event(vector<class CGameObject*> DeadObjs)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& dead : DeadObjs)
		{
			for (auto& Pair : m_pLayers[i])
				Pair.second->Delete_Object(dead);
		}
	}

	return S_OK;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed To Created : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CObject_Manager::Free()
{
	
	for (_uint i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])
		{
			Safe_Release(Pair.second);
		}
		m_pLayers[i].clear();
		
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
