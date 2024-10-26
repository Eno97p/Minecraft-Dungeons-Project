#pragma once
#include "Component.h"
BEGIN(Engine)
class CNode;
END

BEGIN(Engine)

class ENGINE_DLL CBehaviorTree : public CComponent
{
public:
	enum NodeType { Sequence, Selector, Inverter, Action, Node_End};

private:
	CBehaviorTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBehaviorTree(const CBehaviorTree& rhs);
	virtual ~CBehaviorTree() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	void Generate_Root(const wstring& rootName , NodeType _etype);
	void Add_Node(const wstring& parents_name, const wstring& nodename, NodeType _etype,
		function<NodeStates(_float)> action = nullptr , Decorator eDeco = Deco_End, _float _fCooltime = 0.f);
	void			Update(_float fTimeDelta);
	void			Set_Activate(_bool _bool) { m_bActivate = _bool; }
private:
	class CNode* m_RootNode = nullptr;
	map<const wstring, class CNode*>				Nodes;
	_bool m_bActivate = true;

public:
	static CBehaviorTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;


};

END