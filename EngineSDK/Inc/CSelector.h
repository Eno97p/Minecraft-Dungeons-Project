#pragma once
#include"CNode.h"

BEGIN(Engine)
class CSelector final : public CNode
{
private:
	CSelector();
	virtual ~CSelector()=default;
public:
	HRESULT Initialize(Decorator eDecorator, _float fCoolTime);
	virtual void Add_Nodes(CNode* pnode) override {
		m_nodes.push_back(pnode);
	}
private:
	vector<CNode*> m_nodes;

public:
	static CSelector* Create(Decorator eDecorator = Deco_End, _float fCoolTime = 0.f);
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;
};

END