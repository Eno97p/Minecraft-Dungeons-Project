#include "stdafx.h"
#include "..\Public\Body_Player.h"

#include "GameInstance.h"
#include "Player.h"
#include "Arrow.h"
#include "FreeCamera.h"

CBody_Player::CBody_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CPartObject{ pDevice, pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player & rhs)
	: CPartObject{ rhs }
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBody_Player::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;	

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(18, true));	

	return S_OK;
}

void CBody_Player::Priority_Tick(_float fTimeDelta)
{
}

void CBody_Player::Tick(_float fTimeDelta)
{
	CModel::ANIMATION_DESC		AnimDesc{3, true};

	if (*m_pState == CPlayer::STATE_IDLE)
	{
		AnimDesc.isLoop = true;

		if(*m_pWeapon == CPlayer::E_KATANA)
			AnimDesc.iAnimIndex = 25;
		else if(*m_pWeapon == CPlayer::E_HAMMER)
			AnimDesc.iAnimIndex = 24;
		else if (*m_pWeapon == CPlayer::E_Claymore)
			AnimDesc.iAnimIndex = 22;
		else
			AnimDesc.iAnimIndex = 20;

		m_fAnimSpeed = 1.f;
	}

	if (*m_pState == CPlayer::STATE_RUN)
	{


		AnimDesc.isLoop = true;
		if (*m_pWeapon == CPlayer::E_KATANA)
			AnimDesc.iAnimIndex = 34;
		else if (*m_pWeapon == CPlayer::E_HAMMER)
			AnimDesc.iAnimIndex = 33;
		else
			AnimDesc.iAnimIndex = 31;
	
		//if (!Step[0] && m_pModelCom->Get_Current_Ratio() > 0.3f)
		//{
		//	StepSound();
		//	Step[0] = true;
		//}
		//if (!Step[1] && m_pModelCom->Get_Current_Ratio() > 0.6f)
		//{
		//	StepSound();
		//	Step[1] = true;
		//}

		
		StepSound(fTimeDelta);



		m_fAnimSpeed = 1.f;
	}

	if (*m_pState == CPlayer::STATE_DASH)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 9;
		m_fAnimSpeed = 2.0f;
	}

	if (*m_pState == CPlayer::STATE_HIT)
	{
		AnimDesc.isLoop = false;
		AnimDesc.iAnimIndex = 16;
		m_fAnimSpeed = 3.f;
	}

	if (*m_pState == CPlayer::STATE_SHOOT)
	{
		m_fAnimSpeed = 4.f;
		AnimDesc.iAnimIndex = 1;
		AnimDesc.isLoop = false;
	}

	if (*m_pState == CPlayer::STATE_DEAD)
	{
		m_fAnimSpeed = 1.f;
		AnimDesc.iAnimIndex = 2;
		AnimDesc.isLoop = false;
	}

	if (*m_pState == CPlayer::STATE_REVIVE)
	{
		m_fAnimSpeed = 1.f;
		AnimDesc.iAnimIndex = 40;
		AnimDesc.isLoop = false;
	}


	if (*m_pState == CPlayer::STATE_ATTACK)
	{
		AnimDesc.isLoop = false;
		switch (*m_pWeapon)
		{
		case CPlayer::E_SWORD:
			m_fAnimSpeed = 2.f;
			AnimDesc.iAnimIndex = 42;
			break;
		case CPlayer::E_DOUBLEAXE:
			m_fAnimSpeed = 2.f;
			AnimDesc.iAnimIndex = 3;
			break;
		case CPlayer::E_SPEAR:
			m_fAnimSpeed = 1.8f;
			AnimDesc.iAnimIndex = 41;
			break;
		case CPlayer::E_AXE:
			m_fAnimSpeed = 1.8f;
			AnimDesc.iAnimIndex = 0;
			break;
		case CPlayer::E_KATANA:
			m_fAnimSpeed = 2.f;
			AnimDesc.iAnimIndex = 4;
			break;
		case CPlayer::E_HAMMER:
			m_fAnimSpeed = 2.f;
			AnimDesc.iAnimIndex = 17;
			break;
		case CPlayer::E_Claymore:
			m_fAnimSpeed = 3.f;
			AnimDesc.iAnimIndex = 8;
			break;
		}

	}
	m_pModelCom->Set_AnimationIndex(AnimDesc);
	m_pModelCom->Play_Animation(fTimeDelta * m_fAnimSpeed);
	
}

void CBody_Player::Late_Tick(_float fTimeDelta)
{
	XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(m_pParentMatrix));
	//UpdateOutlineMat();
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
}

HRESULT CBody_Player::Render()
{
	

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;		

		m_pShaderCom->Begin(ComputeZ());

		m_pModelCom->Render(i);
	}

	//Render_OutLine();
	
	return S_OK;
}

HRESULT CBody_Player::Render_LightDepth()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	/* 광원 기준의 뷰 변환행렬. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(68.f, 150.f, -126.f, 0.f), XMVectorSet(83.f, 24.f, -36.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CBody_Player::Render_OutLine()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &OutlineMat)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", false))) //맞았을때
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(3);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

void CBody_Player::UpdateOutlineMat()
{
	// 외곽선 변환 행렬 계산
	_float4x4 scaleMat, translateMat;
	_float4x4 World = m_WorldMatrix;
	_matrix TransMat;
	_float outlineScale = 1.1f; // 외곽선의 크기 조절 값
	_float outlineOffset = 0.5f; // 외곽선의 위치 조절 값

	// 크기 조절 행렬 계산
	XMStoreFloat4x4(&scaleMat, XMMatrixScaling(outlineScale, outlineScale, outlineScale));

	// 객체의 회전을 고려하여 뒤쪽 방향 벡터 계산
	_vector Campos = m_pGameInstance->Get_CamPosition();
	_vector CamLook = XMVector3Normalize(Campos - XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43,1.f)) *-1.f;
	

	//_vector objectBackward = XMVector3Normalize(XMVectorSet(-m_WorldMatrix._31, -m_WorldMatrix._32, -m_WorldMatrix._33, 0.0f));
	_vector outlineTranslation = CamLook * outlineOffset;
	XMStoreFloat4x4(&translateMat, XMMatrixTranslationFromVector(outlineTranslation));



	// 월드 행렬에 크기와 위치 조절 행렬을 적용하여 외곽선 변환 행렬 계산
	XMStoreFloat4x4(&OutlineMat, XMLoadFloat4x4(&m_WorldMatrix) * XMLoadFloat4x4(&scaleMat) * XMLoadFloat4x4(&translateMat));


}

_bool CBody_Player::IsAnimFinished()
{
	return m_pModelCom->Get_AnimFinished();
}

_uint CBody_Player::ComputeZ()
{
	
	_vector Campos = m_pGameInstance->Get_CamPosition();

	ID3D11Texture2D* m_pTexture2D = m_pGameInstance->Get_DepthTexture();

	D3D11_MAPPED_SUBRESOURCE		SubResources{};

	_vector vPos = XMVectorSet(m_WorldMatrix._41, m_WorldMatrix._42, m_WorldMatrix._43, 1.f);


	vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
 	vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));
	
	
	if (!static_cast<CFreeCamera*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), "Free_Camera"))->Get_Aproack())
		return 2;

	_float4 finalPos;
	XMStoreFloat4(&finalPos, vPos);

	m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResources);

	_uint		iIndex = 360 * (g_iWinSizeX)+640;
	_float4		vResult = ((_float4*)SubResources.pData)[iIndex];

	m_pContext->Unmap(m_pTexture2D, 0);

	if (0.0f == vResult.w)
		return 2;

	if (abs(vResult.x - finalPos.z) > 0.005f)
	{
		return 3;
	}
	else
		return 2;
}

HRESULT CBody_Player::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
		return E_FAIL;	
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", false))) //맞았을때
		return E_FAIL;


	return S_OK;
}

void CBody_Player::StepSound(_float fTimeDelta)
{
	static _float Steptime = 0.3f;
	Steptime -= fTimeDelta;
	if (Steptime < 0.f)
	{
		_int RandomSound = RandomInt(0, 4);
		switch (RandomSound)
		{
		case 0:
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_stepGeneric-005_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			break;
		case 1:
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_stepGeneric-004_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			break;
		case 2:
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_stepGeneric-003_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			break;
		case 3:
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_stepGeneric-002_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			break;
		case 4:
			m_pGameInstance->PlaySound_Z(_T("sfx_multi_stepGeneric-001_soundWave.ogg"), SOUND_MONSTER, 0.3f);
			break;
		}

		Steptime = 0.3f;
	}


}

CBody_Player * CBody_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBody_Player*		pInstance = new CBody_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBody_Player::Clone(void * pArg)
{
	CBody_Player*		pInstance = new CBody_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBody_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBody_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
