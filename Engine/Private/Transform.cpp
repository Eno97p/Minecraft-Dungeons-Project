#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "GameInstance.h"
CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent{ pDevice, pContext }
{
}

CTransform::CTransform(const CTransform & rhs)
	: CComponent{ rhs }
{
}



void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}



HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		TRANSFORM_DESC*		pTransformDesc = (TRANSFORM_DESC*)pArg;

		m_fSpeedPerSec = pTransformDesc->fSpeedPerSec;
		m_fRotationPerSec = pTransformDesc->fRotationPerSec;
	}

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShader, const _char * pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);	
}

void CTransform::Scaling(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, Get_State(STATE_RIGHT) * fScaleX);
	Set_State(STATE_UP, Get_State(STATE_UP) * fScaleY);
	Set_State(STATE_LOOK, Get_State(STATE_LOOK) * fScaleZ);
}

void CTransform::MoveDirection(_vector vDir, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);

	vPosition += XMVector3Normalize(vDir) * 2.f * fTimeDelta;
	if (nullptr == pNavigation ? true : pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);
	
}

void CTransform::MoveDirection(_vector vDir, _float fTimeDelta, _float fSpeed, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);

	vPosition += XMVector3Normalize(vDir) * fSpeed * fTimeDelta;
	if (nullptr == pNavigation ? true : pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);

}

void CTransform::Set_Scale(_float fScaleX, _float fScaleY, _float fScaleZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fScaleX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fScaleY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fScaleZ);
}

void CTransform::Move_To(_vector Axis, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);
	_float3 scale = Get_Scaled();

	if (XMVector4Equal(XMVector4Normalize(Axis), XMVector4Normalize(vLook)))
	{
		Go_Straight(fTimeDelta, pNavigation);
		return;
	}
	else
	{
		Set_State(STATE_LOOK, Axis* scale.z);
	}

	vPosition += XMVector3Normalize(Axis) * m_fSpeedPerSec * fTimeDelta;

	if (nullptr == pNavigation ? true : pNavigation->isMove(vPosition))
		Set_State(STATE_POSITION, vPosition);

}

void CTransform::Jump(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vUp = Get_State(STATE_UP);

	m_CurPower -= fTimeDelta * 10.f;
	vPosition += vUp * m_CurPower * fTimeDelta;
	Set_State(STATE_POSITION, vPosition);

	if (pNavigation->Get_Height() >= XMVectorGetY(vPosition))
	{
		m_CurPower = m_MaxPower;
		isJump = false;
		pNavigation->Compute_Height(this);
		return;
	}

}

void CTransform::Moving_Lerp(_float fTimeDelta, _vector TargetPos)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector Lerp = XMVectorLerp(vPosition, TargetPos, fTimeDelta * 5.f);
	Set_State(STATE_POSITION, Lerp);
}


void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	_vector vCalPosition = Get_State(STATE_POSITION);
	vCalPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	_float4 NorDir{};
	if (nullptr == pNavigation ? true : pNavigation->isMove(vCalPosition, &NorDir))
		Set_State(STATE_POSITION, vCalPosition);
	else
	{

		_vector SlideDir = XMVector3Normalize(vLook) - XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat4(&NorDir))) * XMVector3Normalize(XMLoadFloat4(&NorDir));
		_vector    SlidePosition = Get_State(STATE_POSITION);
		SlidePosition += XMVector3Normalize(SlideDir) * m_fSpeedPerSec * fTimeDelta;


		if (pNavigation->isMove(SlidePosition))
			Set_State(STATE_POSITION, SlidePosition);
	}

}

void CTransform::Go_Straight_Faster(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	_vector vCalPosition = Get_State(STATE_POSITION);
	vCalPosition += XMVector3Normalize(vLook) * (m_fSpeedPerSec*2.f) * fTimeDelta;

	_float4 NorDir{};
	if (nullptr == pNavigation ? true : pNavigation->isMove(vCalPosition, &NorDir))
		Set_State(STATE_POSITION, vCalPosition);
	else
	{

		_vector SlideDir = XMVector3Normalize(vLook) - XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat4(&NorDir))) * XMVector3Normalize(XMLoadFloat4(&NorDir));
		_vector    SlidePosition = Get_State(STATE_POSITION);
		SlidePosition += XMVector3Normalize(SlideDir) * (m_fSpeedPerSec * 2.f) * fTimeDelta;


		if (pNavigation->isMove(SlidePosition))
			Set_State(STATE_POSITION, SlidePosition);
	}

}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vLook = Get_State(STATE_LOOK);

	_vector vCalPosition = Get_State(STATE_POSITION);
	vCalPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	_float4 NorDir{};
	if (nullptr == pNavigation ? true : pNavigation->isMove(vCalPosition, &NorDir))
		Set_State(STATE_POSITION, vCalPosition);
	else
	{

		_vector SlideDir = XMVector3Normalize(vLook) - XMVector3Dot(XMVector3Normalize(vLook), XMVector3Normalize(XMLoadFloat4(&NorDir))) * XMVector3Normalize(XMLoadFloat4(&NorDir));
		_vector    SlidePosition = Get_State(STATE_POSITION);
		SlidePosition -= XMVector3Normalize(SlideDir) * m_fSpeedPerSec * fTimeDelta;


		if (pNavigation->isMove(SlidePosition))
			Set_State(STATE_POSITION, SlidePosition);
	}


}

void CTransform::Get_Pushed(_vector Axis, _float fTimeDelta, CNavigation* pNavigation)
{
	_vector vPosition = Get_State(STATE_POSITION);
	
	_vector vCalPosition = Get_State(STATE_POSITION);
	vCalPosition -= XMVector3Normalize(Axis) * m_fSpeedPerSec * fTimeDelta;

	//vPosition -= XMVector3Normalize(Axis) * m_fSpeedPerSec * fTimeDelta;
	_float4 NorDir{};
	if (nullptr == pNavigation ? true : pNavigation->isMove(vCalPosition, &NorDir))
		Set_State(STATE_POSITION, vCalPosition);
	else
	{
		_vector SlideDir = XMVector3Normalize(Axis) - XMVector3Dot(XMVector3Normalize(Axis), XMVector3Normalize(XMLoadFloat4(&NorDir))) * XMVector3Normalize(XMLoadFloat4(&NorDir));
		_vector    SlidePosition = Get_State(STATE_POSITION);
		SlidePosition -= XMVector3Normalize(SlideDir) * m_fSpeedPerSec * fTimeDelta;

		if (pNavigation->isMove(SlidePosition))
			Set_State(STATE_POSITION, SlidePosition);
	}
	//if (nullptr == pNavigation ? true : pNavigation->isMove(vPosition))
	//	Set_State(STATE_POSITION, vPosition);

}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);


}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	Set_State(STATE_POSITION, vPosition);

}

void CTransform::LookAt(_fvector vTargetPosition)
{
	_float3 vScaled = Get_Scaled();

	_vector	vLook = vTargetPosition - Get_State(STATE_POSITION);

	_vector	vRight = XMVector3Cross(XMVectorSet(0.0f, 1.f, 0.f, 0.f), vLook);

	_vector vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);

}

void CTransform::LookAt_For_LandObject(_fvector vTargetPosition)
{
	_float3 vScaled = Get_Scaled();

	_vector	vLook = vTargetPosition - Get_State(STATE_POSITION);

	_vector	vRight = XMVector3Cross(XMVectorSet(0.0f, 1.f, 0.f, 0.f), vLook);

	vLook = XMVector3Cross(vRight, XMVectorSet(0.0f, 1.f, 0.f, 0.f));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);	
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);


}

void CTransform::LookAt_Direction(_fvector vTargetDirection)
{
	_float3 vScaled = Get_Scaled();
	
	_vector	vLook = XMVector3Normalize(vTargetDirection);
	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	
	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight );
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook );

}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta, _float fSpeed)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);
	
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fSpeed) * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

}

void CTransform::TurnQuarternion(_fvector vAxis, _float fTimeDelta)
{
	_vector vRight = Get_State(STATE_RIGHT);
	_vector vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	// 쿼터니언을 사용한 회전
	XMVECTOR qRotation = XMQuaternionRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);
	XMVECTOR qRight = XMQuaternionMultiply(XMQuaternionMultiply(qRotation, XMVectorSetW(vRight, 0)), XMQuaternionConjugate(qRotation));
	XMVECTOR qUp = XMQuaternionMultiply(XMQuaternionMultiply(qRotation, XMVectorSetW(vUp, 0)), XMQuaternionConjugate(qRotation));
	XMVECTOR qLook = XMQuaternionMultiply(XMQuaternionMultiply(qRotation, XMVectorSetW(vLook, 0)), XMQuaternionConjugate(qRotation));

	Set_State(STATE_RIGHT, qRight);
	Set_State(STATE_UP, qUp);
	Set_State(STATE_LOOK, qLook);
}

void CTransform::Turn_Slowly(_fvector vAxis, _float fTimeDelta)
{
	if (m_fRotationPerSec == 0.f)
		return;

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);
	m_fRotationPerSec -= fTimeDelta;
	if (m_fRotationPerSec < 0.f)
	{
		m_fRotationPerSec = 0.f;
	}
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

}

_bool CTransform::TurnToTarget(_float fTimeDelta, _fvector vTargetPosition)
{
	_float3 vScaled = Get_Scaled();

	_vector vPos = Get_State(STATE_POSITION);
	_vector	vUp = Get_State(STATE_UP);
	_vector vLook = Get_State(STATE_LOOK);

	
	_vector vTargetLook = XMVectorSetY(XMVectorSubtract(vTargetPosition , vPos),0.f);

	_float fLerp = fTimeDelta * m_fRotationPerSec;

	_vector vCurLook = XMVectorLerp(vLook, vTargetLook, fLerp);

	_float angle = XMVectorGetX(XMVector3Dot(vLook, XMVector3Normalize(vTargetLook)));
	
	if (angle > 0.99f)
	{
		return true;
	}

	_vector	vRight = XMVector3Cross(XMVectorSet(0.0f, 1.f, 0.f, 0.f), vCurLook);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vCurLook) * vScaled.z);

	//플레이어를 정확히 바라볼땐 true를 반환하게 리턴해줘야함
	return false;
}

void CTransform::TurnToRandomValue(_float fTimeDelta, _int random)
{
	_float3 vScaled = Get_Scaled();
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.0f, 1.f, 0.f, 0.f), m_fRotationPerSec * fTimeDelta * random);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp);
	Set_State(STATE_LOOK, vLook);

}

void CTransform::Rotation_Vector(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, XMConvertToRadians(fRadian));
	//XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(fRadian));
	
	_vector		vRight = (Get_State(STATE_RIGHT));
	_vector		vUp = (Get_State(STATE_UP));
	_vector		vLook = (Get_State(STATE_LOOK));
	
	vRight = (XMVector3TransformNormal(vRight, RotationMatrix));
	vUp = (XMVector3TransformNormal(vUp, RotationMatrix));
	vLook = (XMVector3TransformNormal(vLook, RotationMatrix));

	Set_State(STATE_RIGHT, vRight);
	Set_State(STATE_UP, vUp );


}

void CTransform::Rotation_Z(_fvector vAxis, _float fRadian)
{
	_matrix      RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	Set_State(STATE_RIGHT, XMVector3TransformNormal(Get_State(STATE_RIGHT), RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(Get_State(STATE_UP), RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(Get_State(STATE_LOOK), RotationMatrix));

}



void CTransform::BillBoard()
{
	XMVECTOR vPosition = Get_State(STATE_POSITION);
	XMFLOAT4 pos;
	XMStoreFloat4(&pos, vPosition);

	XMFLOAT4X4 matView = *m_pGameInstance->Get_Transform_float4x4_Inverse(CPipeLine::D3DTS_VIEW);
	matView._41 = pos.x;
	matView._42 = pos.y;
	matView._43 = pos.z;
	Set_WorldMatrix(matView);
}





CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*		pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*		pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
