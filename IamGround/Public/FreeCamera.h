#pragma once

#include "Tool_Define.h"
#include "Camera.h"

BEGIN(TOOL)

class CFreeCamera final : public CCamera
{
public:
	typedef struct FCD : public CCamera::CAMERA_DESC
	{
		_float		fSensor = { 0.f };
	}FREE_CAMERA_DESC;
private:
	CFreeCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFreeCamera(const CFreeCamera& rhs);
	virtual ~CFreeCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	_bool Get_Tapped() { return m_bStop; }
private:
	_float		m_fSensor = { 0.0f };
	_bool		m_bStop = false;
public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END