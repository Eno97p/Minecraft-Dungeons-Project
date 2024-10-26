#include "stdafx.h"
#include "HPBar.h"
#include "GameInstance.h"
#include "UIManager.h"

CHPBar::CHPBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CHPBar::CHPBar(const CHPBar& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CHPBar::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HP_Default"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CHPBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", textureChange)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_HP_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffRatio", &m_pDiffRatio, sizeof(_float))))
		return E_FAIL;


	return S_OK;
}




HRESULT CHPBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHPBar::Initialize(void* pArg)
{
	m_PlayerStatus = ((HPBARDESC*)pArg)->pPlayerStat;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;


	m_PreHP = m_PlayerStatus->fMaxHP;

	Set_Unique_ID("HP_Frame");
	return S_OK;
}

void CHPBar::Priority_Tick(_float fTimeDelta)
{


}


void CHPBar::Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
	{
		if (m_PreHP != m_PlayerStatus->fCurHP)
		{
			m_Diff = m_PreHP - m_PlayerStatus->fCurHP;
			textureChange = 1;
		}
		m_Diff -= fTimeDelta * 10.f;
		if (m_Diff < 0.f)
			m_Diff = 0.f;

		if (textureChange == 1)
		{
			Offset -= fTimeDelta;
			if (Offset < 0.f)
			{
				textureChange = 0;
				Offset = 0.1f;
			}

		}
		m_pDiffRatio = (m_PlayerStatus->fCurHP + m_Diff) / m_PlayerStatus->fMaxHP;

		m_fRatio = m_PlayerStatus->fCurHP / m_PlayerStatus->fMaxHP;

		m_PreHP = m_PlayerStatus->fCurHP;
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CHPBar::Late_Tick(_float fTimeDelta)
{
	if (!isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
	
}

HRESULT CHPBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

CHPBar* CHPBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHPBar* pInstance = new CHPBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHPBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHPBar::Clone(void* pArg)
{
	CHPBar* pInstance = new CHPBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHPBar");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CHPBar::Free()
{
	__super::Free();

}
