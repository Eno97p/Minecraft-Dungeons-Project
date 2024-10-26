#pragma once

#include "Tool_Define.h"
#include "Monster.h"

BEGIN(TOOL)

class CSkeleton final : public CMonster
{

private:
	CSkeleton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeleton(const CSkeleton& rhs);
	virtual ~CSkeleton() = default;

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
	static CSkeleton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END