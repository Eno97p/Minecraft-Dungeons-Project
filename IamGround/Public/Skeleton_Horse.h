#pragma once

#include "Tool_Define.h"
#include "Monster.h"

BEGIN(TOOL)

class CSkeletonHorse final : public CMonster
{
private:
	CSkeletonHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkeletonHorse(const CSkeletonHorse& rhs);
	virtual ~CSkeletonHorse() = default;

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
	static CSkeletonHorse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END