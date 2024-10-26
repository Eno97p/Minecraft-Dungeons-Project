#include "stdafx.h"
#include "UIManager.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CUIManager)

CUIManager::CUIManager()
{
}

void CUIManager::Tick(_float fTimeDelta)
{
	for (int i = 0; i < SORT_END; ++i)
	{
		for (auto& iter : m_VecUI[i])
		{
			if (iter == nullptr || iter->Get_Dead())
				continue;
			m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, iter);
		}
	}
	Clear();
}

HRESULT CUIManager::Add_RenderUI(CGameObject* ui, UISORTTYPE type)
{
	Safe_AddRef(ui);
	m_VecUI[type].emplace_back(ui);
	return S_OK;
}


void CUIManager::Clear()
{
	for (int i = 0; i < SORT_END; ++i)
	{
		for (auto& iter : m_VecUI[i])
		{
			Safe_Release(iter);
		}
		m_VecUI[i].clear();
	}
}

HRESULT CUIManager::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();


	return S_OK;
}


void CUIManager::Free()
{
	m_pGameInstance = nullptr;
}
