#pragma once

#include "Base.h"

BEGIN(Engine)
class CNode abstract : public CBase
{
protected:
	CNode(){};
	virtual ~CNode() = default;

protected:
	NodeStates m_eNodeState = NONE;
	Decorator  m_eDecorator = Deco_End;
	_float m_MaxTime = 0.f; // 쿨다운 시간
	_float m_CurrentCooldown = 0.f; // 현재 쿨다운 시간
public:
	virtual void Add_Nodes(CNode* pnode);
	virtual NodeStates Evaluate(_float fTimeDelta) = 0;

	virtual void Free()override {};
};

END
