#include "..\Public\Layer.h"
#include "GameObject.h"

CLayer::CLayer()
{

}

HRESULT CLayer::Initialize()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.push_back(pGameObject);

	return S_OK;
}

void CLayer::Delete_Object(CGameObject* pGameObject)
{
	if (pGameObject == nullptr)
		return;

	for (auto& iter : m_GameObjects)
	{
		if (iter == pGameObject)
		{
			Safe_Release(iter);
			m_GameObjects.remove(iter);
			break;
		}
	}
		
}

void CLayer::Delete_All()
{
	if (m_GameObjects.size() < 1)
		return;

	for (auto& iter : m_GameObjects)
	{
		Safe_Release(iter);
	}
	m_GameObjects.clear();
}

void CLayer::Priority_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead())
			continue;
		pGameObject->Priority_Tick(fTimeDelta);
	}
}

void CLayer::Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead())
			continue;
		pGameObject->Tick(fTimeDelta);
	}
}

void CLayer::Late_Tick(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject->Get_Dead())
			continue;
		pGameObject->Late_Tick(fTimeDelta);
	}
}

CGameObject* CLayer::Find_Obj(CGameObject* pfind)
{
	if (pfind == nullptr)
		return nullptr;

	for (auto& pGameObject : m_GameObjects)
	{
		if (pGameObject == pfind)
		{
			return pGameObject;
		}
	}
}

CGameObject* CLayer::Find_GameObject_From_ID(const char* _ID)
{
	for (auto gameObject : m_GameObjects) {
		if (gameObject->Get_Unique_ID() == _ID) {
			return gameObject;
		}
	}
	return nullptr;
}

CGameObject* CLayer::Get_Object(_uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return *iter;
}




CLayer * CLayer::Create()
{
	CLayer*		pInstance = new CLayer();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CLayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);

	m_GameObjects.clear();
}