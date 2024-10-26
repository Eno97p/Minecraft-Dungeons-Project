#include "stdafx.h"
#include "..\Public\FreeCamera.h"

#include "GameInstance.h"
#include "Mouse.h"

CFreeCamera::CFreeCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCamera{ pDevice, pContext }
{
}

CFreeCamera::CFreeCamera(const CFreeCamera & rhs)
	: CCamera{ rhs }
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFreeCamera::Initialize(void * pArg)
{
	FREE_CAMERA_DESC*		pDesc = (FREE_CAMERA_DESC*)pArg;

	m_fSensor = pDesc->fSensor;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	ZeroMemory(&vEye, sizeof(_float4));
	ZeroMemory(&vAt, sizeof(_float4));
	ZeroMemory(&vDir, sizeof(_float4));

	fDistance = 50.f;
	//fDistance = 6.f;
	m_fAngle = 270.f;
	//m_fAngle = 180.f;
	fSpeed = 12.f;
	Set_Unique_ID("Free_Camera");

	return S_OK;
}

void CFreeCamera::Priority_Tick(_float fTimeDelta)
{
}

void CFreeCamera::Tick(_float fTimeDelta)
{
	if (m_pTarget == nullptr)
		return;

	if (ChangeTargetPoint)
	{
		fStartLerp += fTimeDelta * 0.002f;

		_vector CurAt = XMLoadFloat4(&vAt);
		vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
		vEye.y = vAt.y + fDistance;
		vEye.z = vAt.z + sinf(m_fAngle) * fDistance;
		vEye.w = 1.f;

		_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector Dst = XMLoadFloat4(&vEye);


		_vector Lerp = XMVectorLerp(CurPos, Dst, fStartLerp);
		_vector LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), fStartLerp);

		if (XMVectorGetX(XMVector3Length(Dst - Lerp)) < 0.01f)
			m_isTargetChanging = false;



		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);
		m_pTransformCom->LookAt(LerpAt);


		if (m_fShakeTime > m_fShakeAccTime)
		{
			m_fShakeAccTime += fTimeDelta;
			Shaking();
		}

		__super::Tick(fTimeDelta);
		return;
	}


	if (m_isCutScene)
	{
		fStartLerp += fTimeDelta * 0.002f;

		_vector vTarget;
		CTransform* TargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
		vTarget = TargetTransform->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&vAt, vTarget); //vTarget이 플레이어의 위치벡터. vAt은 플레이어를 바라본다.

		_vector CurAt = XMLoadFloat4(&vAt);
		vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
		vEye.y = vAt.y + fDistance;
		vEye.z = vAt.z + sinf(m_fAngle) * fDistance;
		vEye.w = 1.f;

		_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector Dst = XMLoadFloat4(&vEye);


		_vector Lerp = XMVectorLerp(CurPos, Dst, fStartLerp);
		_vector LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), 0.000001f);

	
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);
		m_pTransformCom->LookAt(LerpAt);

	}
	else
	{
		if (!m_isTargetChanging)			//여기만 보셈
		{
			_vector vTarget;		//바라볼 타겟
			CTransform* TargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			vTarget = TargetTransform->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&vAt, vTarget); //	vTarget이 플레이어의 위치벡터. vAt은 플레이어의 위치

			_vector CurAt = XMLoadFloat4(&vAt);
			vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
			vEye.y = vAt.y + fDistance;					//y축을 따로 더 높이고 싶다면 다른 멤버변수 주면 됨.
			vEye.z = vAt.z + sinf(m_fAngle) * fDistance;  //m_fAngle을 높이거나 줄이면 타깃을 기준으로 좌 우로 공전함
			vEye.w = 1.f;

			_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);	//현재 카메라 위치
			_vector Dst = XMLoadFloat4(&vEye);		//카메라가 가야 할 위치


			_vector Lerp = XMVectorLerp(CurPos, Dst, 0.5f);			//현재 카메라 위치와 카메라가 가야할 위치를 선형보간
			LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), 0.5f);		//현재 카메라가 바라보는위치와 바라봐야할 위치 또한 선형보간


			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);	//선형보간된 값으로 셋해준다
			m_pTransformCom->LookAt(LerpAt);		//바라봐야할 위치도 선형보간된 값으로 셋해준다

			if (abs(XMVectorGetX(XMVector3Length(LerpAt - vTarget))) < 0.1f)	//이거는 선형보관값이 일정 수치 이하일때 멈춤
				IsAprroack = true;
			else
				IsAprroack = false;


		}
		else
		{
			_vector vTarget;
			CTransform* TargetTransform = static_cast<CTransform*>(m_pTarget->Get_Component(TEXT("Com_Transform")));
			vTarget = TargetTransform->Get_State(CTransform::STATE_POSITION);
			XMStoreFloat4(&vAt, vTarget); //vTarget이 플레이어의 위치벡터. vAt은 플레이어를 바라본다.

			_vector CurAt = XMLoadFloat4(&vAt);
			vEye.x = vAt.x + cosf(m_fAngle) * fDistance; //카메라의 각도와 위치 조정
			vEye.y = vAt.y + fDistance;
			vEye.z = vAt.z + sinf(m_fAngle) * fDistance;
			vEye.w = 1.f;

			_vector CurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector Dst = XMLoadFloat4(&vEye);


			_vector Lerp = XMVectorLerp(CurPos, Dst, fTimeDelta);
			_vector LerpAt = XMVectorLerp(CurAt, XMLoadFloat4(&vAt), fTimeDelta);

			if (XMVectorGetX(XMVector3Length(Dst - Lerp)) < 0.01f)
				m_isTargetChanging = false;



			m_pTransformCom->Set_State(CTransform::STATE_POSITION, Lerp);
			m_pTransformCom->LookAt(LerpAt);
		}
		if (m_fShakeTime > m_fShakeAccTime)
		{
			m_fShakeAccTime += fTimeDelta;
			Shaking();
		}

	}
	__super::Tick(fTimeDelta);
}

void CFreeCamera::Late_Tick(_float fTimeDelta)
{
	Key_Input(fTimeDelta);
}

HRESULT CFreeCamera::Render()
{
	return S_OK;
}

void CFreeCamera::Change_Target(CGameObject* m_Target)
{
	m_isTargetChanging = true;
	m_pTarget = m_Target;
}

void CFreeCamera::CutScene(CGameObject* m_Target)
{
	m_isCutScene = true;
	m_pTarget = m_Target;
}

void CFreeCamera::Set_Shop()
{
	CGameObject* Shopkeeper = m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Frendly"), "Chair");
	Set_Target(Shopkeeper);

	m_fAngle = 95.f;
	fDistance = 1.6f;

}

void CFreeCamera::Shaking()
{
	_vector vLook;
	_vector vRight;
	_vector vUp;
	
	vLook = XMVector3Normalize(XMLoadFloat4(&vAt) - XMLoadFloat4(&vEye));
	vRight = XMVector3Cross(vLook, m_pTransformCom->Get_State(CTransform::STATE_UP));
	vUp = XMVector3Cross(vLook, vRight);

	// -1.5f ~ 1.5f 사이의값 구함
	float offsetY = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
	_vector eye = XMLoadFloat4(&vEye);
	_vector at = XMLoadFloat4(&vAt);
	eye += vUp * offsetY;
	at += vUp * offsetY;

	if (!m_bLockWidth)
	{
		float offsetX = ((rand() % 100 / 100.0f) * m_fIntensity) - (m_fIntensity * 0.5f);
		eye += vRight * offsetX;
		at += vRight * offsetX;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, eye);
	m_pTransformCom->LookAt(at);
	
}



void CFreeCamera::Key_Input(_float fTimeDelta)
{
	if (m_pGameInstance->CheckMouseWheel() == WHEEL_UP)
		fDistance -= 10.f * fTimeDelta;
	else if (m_pGameInstance->CheckMouseWheel() == WHEEL_DOWN)
		fDistance += 10.f * fTimeDelta;
	

	if (GetAsyncKeyState('O'))
	{
		m_fAngle += fTimeDelta;
	}


	if (GetAsyncKeyState('P'))
	{
		m_fAngle -= fTimeDelta;
	}


}





CFreeCamera * CFreeCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFreeCamera*		pInstance = new CFreeCamera(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CFreeCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFreeCamera::Clone(void * pArg)
{
	CFreeCamera*		pInstance = new CFreeCamera(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFreeCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFreeCamera::Free()
{
	__super::Free();

}
