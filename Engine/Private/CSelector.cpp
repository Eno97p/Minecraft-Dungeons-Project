
#include "CSelector.h"

CSelector::CSelector()
{
}

HRESULT CSelector::Initialize(Decorator eDecorator ,_float fCoolTime)
{
	m_MaxTime = fCoolTime;
	m_eDecorator = eDecorator;
	return S_OK;
}

NodeStates CSelector::Evaluate(_float fTimeDelta)
{
	if (m_eDecorator == CoolDown)
	{
		if (m_CurrentCooldown > 0.0f)
		{
			m_CurrentCooldown -= fTimeDelta;
			m_eNodeState = NodeStates::COOLING; // ��ٿ� ���� RUNNING ���·� ����
			return m_eNodeState;
		}
	}


	for (auto& node : m_nodes)
	{
		switch (node->Evaluate(fTimeDelta))
		{
		case NodeStates::FAILURE:
			continue;
		case NodeStates::SUCCESS:
			m_CurrentCooldown = m_MaxTime;
			m_eNodeState = NodeStates::SUCCESS;
			return m_eNodeState;             //�ڽ� ���� �� �ϳ��� ture�̸� Selector�� true�� ����. �׸��� �ٷ� ����
		case NodeStates::RUNNING:
			m_eNodeState = NodeStates::RUNNING;
			return m_eNodeState;
		case NodeStates::COOLING:
			continue;
		default:
			continue;
		}

	}
	m_eNodeState = NodeStates::FAILURE;
	return m_eNodeState;

}

CSelector* CSelector::Create(Decorator eDecorator, _float fCoolTime)
{
	CSelector* pInstance = new CSelector();
	if (FAILED(pInstance->Initialize(eDecorator, fCoolTime)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CSelector");
		return nullptr;
	}
	return pInstance;


}

void CSelector::Free()
{
	for(auto& node : m_nodes)
	{
		Safe_Release(node);
	}
	m_nodes.clear();
}
