#include "stdafx.h"
#include "BossHpBar.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inventory.h"
CBossHpBar::CBossHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CBossHpBar::CBossHpBar(const CBossHpBar& rhs)
	: CUI{ rhs }
{
}

HRESULT CBossHpBar::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossHpBar"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHPBar"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CBossHpBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_HP_Ratio", &m_pRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffRatio", &m_pDiffRatio, sizeof(_float))))
		return E_FAIL;
	// 이거 조정하면 됨
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	return S_OK;
}


HRESULT CBossHpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossHpBar::Initialize(void* pArg)
{
	m_CurHp = ((CBOSSBARDESC*)pArg)->CurrentHp;
	m_MaxHp = ((CBOSSBARDESC*)pArg)->MaxHp;
	FontString = ((CBOSSBARDESC*)pArg)->FontString;
	m_FontPos = ((CBOSSBARDESC*)pArg)->FontPos;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = 815;
	m_fSizeY = 20.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;


	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY *0.8f, 0.f, 1.f));

	m_fX = m_fX - g_iWinSizeX * 0.5f;
	m_fY = -m_fY + g_iWinSizeY * 0.8f;

	m_PreHp = *m_CurHp;

	Set_Unique_ID("BossHpBar");
	return S_OK;
}

void CBossHpBar::Priority_Tick(_float fTimeDelta)
{


}

void CBossHpBar::Tick(_float fTimeDelta)
{
	if (m_PreHp != *m_CurHp)
	{
		m_Diff = m_PreHp - *m_CurHp;
	}
	m_Diff -= fTimeDelta * 10.f;
	if (m_Diff < 0.f)
		m_Diff = 0.f;

	m_pDiffRatio = (*m_CurHp + m_Diff) / *m_MaxHp;

	*m_CurHp = max(*m_CurHp, 0.0f);
	m_pRatio = *m_CurHp / *m_MaxHp;

	if (*m_CurHp <= 0.f)
		m_pGameInstance->Erase(this);
	m_PreHp = *m_CurHp;

	__super::Tick(fTimeDelta);
}

void CBossHpBar::Late_Tick(_float fTimeDelta)
{
	if (*CInventory::GetInstance()->Get_InvenOpen() == false)
	{
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
	}
}

HRESULT CBossHpBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);
	
	m_pVIBufferCom->Bind_Buffers();

	

	m_pVIBufferCom->Render();
	//_float2(g_iWinSizeX / 2 - 310.f, 50.f)
	m_pGameInstance->Render_Font(TEXT("Font_Default"), FontString, m_FontPos, XMVectorSet(1.f, 1.f, 1.f, 1.f));
	
	return S_OK;
}

CBossHpBar* CBossHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossHpBar* pInstance = new CBossHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Mouse");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossHpBar::Clone(void* pArg)
{
	CBossHpBar* pInstance = new CBossHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Mouse");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CBossHpBar::Free()
{
	__super::Free();

}
