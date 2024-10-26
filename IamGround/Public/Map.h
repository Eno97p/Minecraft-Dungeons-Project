#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CNavigation;
END

BEGIN(TOOL)

class CMap : public CGameObject
{
public:
	typedef struct MAP_DEC
	{
		_float3 Scale;
		_float4 Pos;
	};

protected:
	CMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	MAP_DEC Get_MapDec() {
		return m_Mapdec;
	}
	virtual _bool Mesh_Picking(_float3* PickingPoint) override;

protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CNavigation*				m_pNavigationCom = { nullptr };
protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ShaderResources() = 0;
	
protected:
	_float3 m_Scale;
	MAP_DEC m_Mapdec = {};
public:
	virtual void Free() override;
};

END