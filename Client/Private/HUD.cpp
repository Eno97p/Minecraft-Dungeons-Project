#include "stdafx.h"
#include "HUD.h"

#include "GameInstance.h"
#include "UIManager.h"
#include "Inventory.h"

CHUD::CHUD(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CHUD::CHUD(const CHUD & rhs)
	: CUI(rhs)
{
}

HRESULT CHUD::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHUD::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;
	//m_MaxHp = ((HUDDESC*)pArg)->MaxHp;
	//m_CurHp = ((HUDDESC*)pArg)->CurrentHp;

	m_PlayerStatus = ((HUDDESC*)pArg)->pPlayerStat;




	m_PreHP = m_PlayerStatus->fCurHP;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	return S_OK;
}

void CHUD::Priority_Tick(_float fTimeDelta)
{
	
}

void CHUD::Tick(_float fTimeDelta)
{
	_float Diff = 0.f;
	if (m_PreHP != m_PlayerStatus->fCurHP && m_PlayerStatus->fCurHP < m_PreHP)
	{
		Diff = m_PreHP - m_PlayerStatus->fCurHP;
		if (Diff >= 10.f)
		{
			m_ResetTime = 1.f;
		}
	}




	m_ResetTime -= fTimeDelta;
	if (m_ResetTime < 0.f)
		m_ResetTime = 0.f;


	if (m_PreHP < 30.f)
		m_ResetTime = 1.f;

	m_pRatio = m_ResetTime / 1.f;

	m_PreHP = m_PlayerStatus->fCurHP;

}

void CHUD::Late_Tick(_float fTimeDelta)
{
	if (*CInventory::GetInstance()->Get_InvenOpen() == true)
		return;

	CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
}

HRESULT CHUD::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(3);

	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CHUD::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HUD"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;	
	
	return S_OK;
}

HRESULT CHUD::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffRatio", &m_pRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CHUD * CHUD::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHUD*		pInstance = new CHUD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHUD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHUD::Clone(void * pArg)
{
	CHUD*		pInstance = new CHUD(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHUD");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHUD::Free()
{
	__super::Free();

}
