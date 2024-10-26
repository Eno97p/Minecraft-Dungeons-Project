#pragma once
#include"CNode.h"
BEGIN(Engine)
class CActionNode final : public CNode	
{
private:
	CActionNode(function<NodeStates(_float fTimeDelta)> action);
	virtual ~CActionNode() = default;
public:
	HRESULT Initialize(Decorator eDecorator, _float fCoolTime);
private:
	function<NodeStates(_float fTimeDelta)> m_action;

public:
	static CActionNode* create(function<NodeStates(_float fTimeDelta)> action , Decorator eDecorator = Deco_End, _float fCoolTime = 0.f);
	virtual NodeStates Evaluate(_float fTimeDelta) override;
	virtual void Free() override;
};

END