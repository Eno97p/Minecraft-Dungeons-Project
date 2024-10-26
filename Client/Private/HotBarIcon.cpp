#include "stdafx.h"
#include "HotBarIcon.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inven_Item_Info.h"
#include "Inventory.h"

CHotBarIcon::CHotBarIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CHotBarIcon::CHotBarIcon(const CHotBarIcon& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CHotBarIcon::Add_Components(const wchar_t* TextureTag)
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TextureTag,
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHotBarIcon::Bind_ShaderResources()
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




HRESULT CHotBarIcon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHotBarIcon::Initialize(void* pArg)
{

	m_Font = ((HOTBARICON*)pArg)->Font;
	FontPosition = ((HOTBARICON*)pArg)->FontPosition;
	if (((HOTBARICON*)pArg)->Value != nullptr)
	{
		m_Value = ((HOTBARICON*)pArg)->Value;
		FontPosition2 = ((HOTBARICON*)pArg)->Font2Position;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components(((HOTBARICON*)pArg)->TextureTag)))
		return E_FAIL;

	Set_Unique_ID("Hot_Bar_Icon");

	return S_OK;
}

void CHotBarIcon::Priority_Tick(_float fTimeDelta)
{


}


void CHotBarIcon::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CHotBarIcon::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
	{
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
	}
}

HRESULT CHotBarIcon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	m_pGameInstance->Render_Font(TEXT("Font_Middle"), m_Font, _float2(m_fX + FontPosition.x, m_fY + FontPosition.y), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	if(m_Value != nullptr)
		m_pGameInstance->Render_Font(TEXT("Font_Middle"), to_wstring(*m_Value), _float2(m_fX + FontPosition2.x, m_fY + FontPosition2.y), XMVectorSet(1.f, 1.f, 1.f, 1.f));


	return S_OK;
}

CHotBarIcon* CHotBarIcon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHotBarIcon* pInstance = new CHotBarIcon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHotBarIcon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHotBarIcon::Clone(void* pArg)
{
	CHotBarIcon* pInstance = new CHotBarIcon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHotBarIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHotBarIcon::Free()
{
	__super::Free();
}
