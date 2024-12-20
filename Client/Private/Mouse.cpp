#include "stdafx.h"
#include "Mouse.h"
#include "GameInstance.h"
#include "UIManager.h"
CMouse::CMouse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CMouse::CMouse(const CMouse& rhs)
	: CUI{ rhs }
{
}

HRESULT CMouse::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMouse::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	// 이거 조정하면 됨
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}


HRESULT CMouse::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMouse::Initialize(void* pArg)
{
	ShowCursor(false);
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 20.f;
	m_fSizeY = 20.f;


	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	ZeroMemory(&m_vMousePos, sizeof(_float4));
	Set_Unique_ID("Mouse");
	return S_OK;
}

void CMouse::Priority_Tick(_float fTimeDelta)
{


}

void CMouse::Tick(_float fTimeDelta)
{
	POINT mouse = m_pGameInstance->Get_Mouse_Point();

	_vector		vMousePos = XMVectorZero();
	vMousePos = XMVectorSetX(vMousePos, -(g_iWinSizeX / 2.f) + mouse.x);
	vMousePos = XMVectorSetY(vMousePos, (g_iWinSizeY / 2.f) - mouse.y);
	vMousePos = XMVectorSetZ(vMousePos, 0.f);
	vMousePos = XMVectorSetW(vMousePos, 1.f);
	XMStoreFloat4(&m_vMousePos, vMousePos);
	m_fX = mouse.x;
	m_fY = mouse.y;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMousePos);

	__super::Tick(fTimeDelta);
}

void CMouse::Late_Tick(_float fTimeDelta)
{
	CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::NINETH);
}

HRESULT CMouse::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CMouse* CMouse::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMouse* pInstance = new CMouse(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Mouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMouse::Clone(void* pArg)
{
	CMouse* pInstance = new CMouse(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Mouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CMouse::Free()
{
	__super::Free();

	//Safe_Release(m_pVIBufferCom);
	//Safe_Release(m_pTextureCom);
	//Safe_Release(m_pShaderCom);
	ShowCursor(true);
}
