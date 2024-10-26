#pragma once

#include "Tool_Define.h"
#include "Map.h"


BEGIN(TOOL)

class CMapTest final : public CMap
{

private:
	CMapTest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapTest(const CMapTest& rhs);
	virtual ~CMapTest() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	

public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ShaderResources() override;


public:
	static CMapTest* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END