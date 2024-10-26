#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

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
	void Set_Target(class CGameObject* m_Target)
	{
		m_pTarget = m_Target;
		m_isTargetChanging = false;
	}
	void Change_Target(class CGameObject* m_Target);
	void CutScene(class CGameObject* m_Target);
	void Set_CutSceneEnd() { m_isCutScene = false; }
	void Distance_Up(_float fTimeDelta)
	{
		fDistance += fTimeDelta;
	}
	void Angle_Up(_float fTimeDelta)
	{
		m_fAngle += fTimeDelta *0.2f;
	}
	void Angle_Down(_float fTimeDelta)
	{
		m_fAngle -= fTimeDelta * 0.2f;
	}
	void Reset_Distance()
	{
		fDistance = 6.f;
		m_fAngle = 180.f;
		ChangeTargetPoint = false;
		fStartLerp = 0.f;
	}

	void LerpAngle(_float _angle, _float fTimeDelta)
	{
		m_fAngle = LerpFloat(m_fAngle, _angle, fTimeDelta);
	}
	void SetAngle(_float _angle)
	{
		m_fAngle = _angle;
	}
	void Set_ChangeTarget_Point(_vector vPoint)
	{
		ChangeTargetPoint = true;
		XMStoreFloat4(&vAt, vPoint);
	}
	void Set_Shop();

public:
	void Shaking();
	void Set_ShakeCamera(_float _fIntensity, _float _fShakeTime, _bool _bLockWidth)
	{
		m_fShakeAccTime = 0.f;
		m_fIntensity = _fIntensity;
		m_fShakeTime = _fShakeTime;
		m_bLockWidth = _bLockWidth;
	}

	void Key_Input(_float fTimeDelta);

	_bool Get_Aproack() { return IsAprroack; }
	_vector Get_CurLookAT() { return LerpAt; }
private:
	_float		m_fSensor = { 0.0f };

private:
	class CGameObject* m_pTarget = nullptr;			//Ÿ��
	_float4 vEye;
	_float4 vDir;
	_float	fSpeed = 0.f;
	_float4 vAt;
	_float  fDistance = 0.f;
	_float	m_fHeight = 0.f;
	_float	m_fAngle = 0.f;
	_bool m_bIsMove = false;
	_bool m_bZoom = false;
	_bool m_isCutScene = false;
	_bool ChangeTargetPoint = false;
	_float4 vTargetPoint = {};
	_float fStartLerp = 0.f;
private:
	_bool m_isTargetChanging = false;
	_vector LerpAt;
	_bool IsAprroack = false;
private:
	_float	m_fIntensity;	//ī�޶� ��鸲 ����
	_float	m_fShakeTime;	//���� �ð�
	_float	m_fShakeAccTime;		//���� �ð�
	_bool	m_bLockWidth;
 
public:
	static CFreeCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END