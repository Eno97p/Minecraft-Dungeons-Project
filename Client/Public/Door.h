#pragma once

#include "Client_Defines.h"
#include "Environment.h"

BEGIN(Client)

class CDoor final : public CEnvironment
{
private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDoor(const CDoor& rhs);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Door_Open()
	{
		isDoorOpen = !isDoorOpen;
	}

	
public:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Bind_ShaderResources() override;
private:
	void Summon_Box();
private:
	_bool isDoorOpen = false;
	class CFreeCamera* m_pCamera = nullptr;
	static _int m_iCount;
public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END