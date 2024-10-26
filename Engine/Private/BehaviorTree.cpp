#include "BehaviorTree.h"
#include "CSequence.h"
#include "CSelector.h"
#include "CActionNode.h"
#include "CInverter.h"

CBehaviorTree::CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CComponent(pDevice, pContext)
{
}

CBehaviorTree::CBehaviorTree(const CBehaviorTree& rhs)
	:CComponent(rhs)
{
}

HRESULT CBehaviorTree::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBehaviorTree::Initialize(void* pArg)
{
	return S_OK;
}

void CBehaviorTree::Generate_Root(const wstring& rootName, NodeType _etype)
{
	switch (_etype)
	{
	case Sequence:
		m_RootNode = CSequence::create();
		break;
	case Selector:
		m_RootNode = CSelector::Create();
		break;
	default:
		MSG_BOX("Á¶±î");
		return;
	}
	Nodes.emplace(rootName,m_RootNode);
}

void CBehaviorTree::Add_Node(const wstring& parents_name, const wstring& nodename, NodeType _etype, function<NodeStates(_float)> action, Decorator eDeco, _float _fCooltime)
{
	auto	iter = Nodes.find(parents_name);
	if (iter == Nodes.end())
	{
		MSG_BOX("Failed find Parents");
		return;
	}
	else
	{
		CNode* Node = nullptr;
		switch (_etype)
		{
		case Sequence:
			Node = CSequence::create();
			iter->second->Add_Nodes(Node);
			Nodes.emplace(nodename, Node);
			break;
		case Selector:
			if(eDeco == CoolDown)
				Node = CSelector::Create(eDeco, _fCooltime);
			else
				Node = CSelector::Create();
			iter->second->Add_Nodes(Node);
			Nodes.emplace(nodename, Node);
			break;
		case Inverter:
			Node = CInverter::Create();
			iter->second->Add_Nodes(Node);
			Nodes.emplace(nodename, Node);
			break;
		case Action:
			if (eDeco == CoolDown)
				Node = CActionNode::create(action, eDeco, _fCooltime);
			else
				Node = CActionNode::create(action);
			iter->second->Add_Nodes(Node);
			break;
		}
	}
}

void CBehaviorTree::Update(_float fTimeDelta)
{
	if (m_bActivate)
		m_RootNode->Evaluate(fTimeDelta);
	else
		return;
}


CBehaviorTree* CBehaviorTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBehaviorTree* pInstance = new CBehaviorTree(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Behavior");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CBehaviorTree::Clone(void* pArg)
{
	CBehaviorTree* pInstance = new CBehaviorTree(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Behavior");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBehaviorTree::Free()
{
	Safe_Release(m_RootNode);
	__super::Free();
}
