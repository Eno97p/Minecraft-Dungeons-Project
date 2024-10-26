
#include "CActionNode.h"

CActionNode::CActionNode(function<NodeStates(_float fTimeDelta)> action)
    : m_action(action)
{
}

HRESULT CActionNode::Initialize(Decorator eDecorator, _float fCoolTime)
{
	m_MaxTime = fCoolTime;
	m_eDecorator = eDecorator;
	return S_OK;
}

NodeStates CActionNode::Evaluate(_float fTimeDelta)
{
	if (m_eDecorator == CoolDown)
	{
		if (m_CurrentCooldown > 0.0f)
		{
			m_CurrentCooldown -= fTimeDelta;
			m_eNodeState = NodeStates::COOLING; // 쿨타임 중엔 COOLING을 반환하게
			return m_eNodeState;
		}
	}

	switch (m_action(fTimeDelta))
	{
		case NodeStates::SUCCESS:
			m_CurrentCooldown = m_MaxTime;
			m_eNodeState = NodeStates::SUCCESS;
			return m_eNodeState;
		case NodeStates::FAILURE:
			m_eNodeState = NodeStates::FAILURE;
			return m_eNodeState;
		case NodeStates::RUNNING:
			m_eNodeState = NodeStates::RUNNING;
			return m_eNodeState;
		case NodeStates::COOLING:
			m_eNodeState = NodeStates::COOLING;
			return m_eNodeState;
		default:
			m_eNodeState = NodeStates::FAILURE;
			return m_eNodeState;
	}

}
CActionNode* CActionNode::create(function<NodeStates(_float fTimeDelta)> action, Decorator eDecorator, _float fCoolTime)
{
    CActionNode* pInstance = new CActionNode(action);
	if (FAILED(pInstance->Initialize(eDecorator, fCoolTime)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed To Cloned : CActionNode");
		return nullptr;
	}
	return pInstance;

}


void CActionNode::Free()
{
}
