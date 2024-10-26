#include "stdafx.h"
#include "Emerald_UI.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inventory.h"

CEmerald_UI::CEmerald_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CEmerald_UI::CEmerald_UI(const CEmerald_UI& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CEmerald_UI::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Item_Emerald"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CEmerald_UI::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;



	return S_OK;
}




HRESULT CEmerald_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEmerald_UI::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pNumemerald = CInventory::GetInstance()->Get_NumEmerald();

	Set_Unique_ID("Emerald_UI");
	return S_OK;
}

void CEmerald_UI::Priority_Tick(_float fTimeDelta)
{
}


void CEmerald_UI::Tick(_float fTimeDelta)
{
}


void CEmerald_UI::Late_Tick(_float fTimeDelta)
{
	CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::EIGHT);
}

HRESULT CEmerald_UI::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	std::wstring numberStr = std::to_wstring(*m_pNumemerald);

	m_pGameInstance->Render_Font(TEXT("Font_Middle"), numberStr, _float2(m_fX + 20.f, m_fY - 20.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	return S_OK;
}

CEmerald_UI* CEmerald_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEmerald_UI* pInstance = new CEmerald_UI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CEmerald_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEmerald_UI::Clone(void* pArg)
{
	CEmerald_UI* pInstance = new CEmerald_UI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CEmerald_UI");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CEmerald_UI::Free()
{
	__super::Free();

}
