#include "stdafx.h"
#include "UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUI::CUI(const CUI& rhs)
	: CGameObject{ rhs }
{
}




HRESULT CUI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.0f));

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	m_CollisionRect = { LONG(m_fX - m_fSizeX * 0.5f),
	LONG(m_fY - m_fSizeY * 0.5f),
	LONG(m_fX + m_fSizeX * 0.5f),
	LONG(m_fY + m_fSizeY * 0.5f) };
}

_bool CUI::CollisionRect(RECT* SrcRect)
{
	RECT rt = {};
	return IntersectRect(&rt, &m_CollisionRect, SrcRect);
}

void CUI::Resize(_float4 PositionValue)
{
	m_fX = PositionValue.x;
	m_fY = PositionValue.y;
	m_fSizeX = PositionValue.z;
	m_fSizeY = PositionValue.w;

	_vector		vPos = XMVectorZero();
	vPos = XMVectorSetX(vPos, -(g_iWinSizeX / 2.f) + m_fX);
	vPos = XMVectorSetY(vPos, (g_iWinSizeY / 2.f) - m_fY);
	vPos = XMVectorSetZ(vPos, 0.f);
	vPos = XMVectorSetW(vPos, 1.f);

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}




void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
