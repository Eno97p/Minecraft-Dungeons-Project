#pragma once

#include "Component.h"

/* 객체의 월드 변환 행렬을 보관한다. */
/* 월드 변환 상태를 제어하기위한 기능. */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	typedef struct
	{
		_float		fSpeedPerSec;
		_float		fRotationPerSec;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}

	_float3 Get_Scaled() {
		return _float3(XMVector3Length(Get_State(STATE_RIGHT)).m128_f32[0], 
			XMVector3Length(Get_State(STATE_UP)).m128_f32[0],
			XMVector3Length(Get_State(STATE_LOOK)).m128_f32[0]);
	}

	const _float4x4* Get_WorldFloat4x4() {
		return &m_WorldMatrix;
	}
	_matrix Get_WorldMatrix() {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	_matrix Get_WorldMatrixInverse(){ return XMMatrixInverse(nullptr, Get_WorldMatrix());}

	void Set_WorldMatrix(_float4x4 worldmat) { m_WorldMatrix = worldmat; }

public:
	void Set_State(STATE eState, _fvector vState);
	void Set_Speed(_float2 _speedpair) {
		m_fSpeedPerSec = _speedpair.x;
		m_fRotationPerSec = _speedpair.y;
	}
public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName);

public:
	void Scaling(_float fScaleX, _float fScaleY, _float fScaleZ);
	void MoveDirection(_vector vDir, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void MoveDirection(_vector vDir, _float fTimeDelta, _float fSpeed, class CNavigation* pNavigation = nullptr);
	void Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ);

	void Move_To(_vector Axis, _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Jump(_float fTimeDelta, class CNavigation* pNavigation);
	void Moving_Lerp(_float fTimeDelta, _vector TargetPos);

	void Go_Straight(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Straight_Faster(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Backward(_float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Get_Pushed(_vector Axis , _float fTimeDelta, class CNavigation* pNavigation = nullptr);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);
	void LookAt(_fvector vTargetPosition);
	void LookAt_For_LandObject(_fvector vTargetPosition);
	void LookAt_Direction(_fvector vTargetDirection);

	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fTimeDelta, _float fSpeed);
	void TurnQuarternion(_fvector vAxis, _float fTimeDelta);
	void Turn_Slowly(_fvector vAxis, _float fTimeDelta);
	_bool TurnToTarget(_float fTimeDelta, _fvector vTargetPosition);
	void TurnToRandomValue(_float fTimeDelta, _int random);
	void Rotation(_fvector vAxis, _float fRadian);
	void Rotation_Vector(_fvector vAxis, _float fRadian);

	void Rotation_Z(_fvector vAxis, _float fRadian);

	void Set_Power(_float _fPower) {
		m_MaxPower = _fPower;
		m_CurPower = m_MaxPower;
	}
	void Set_JumpState(_bool bJump) { isJump = bJump; }
	_bool isJumping() { return isJump; }


	void BillBoard();
private:
	/* row major */
	_float4x4				m_WorldMatrix;
	_float					m_fSpeedPerSec = { 0.0f };
	_float					m_fRotationPerSec = { 0.0f };

	_float					m_CurPower = { 0.f };
	_float					m_MaxPower = { 0.f };
	_bool					isJump = false;
public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END