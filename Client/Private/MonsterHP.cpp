#include "stdafx.h"
#include "MonsterHP.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Inventory.h"

CMonsterHpBar::CMonsterHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CMonsterHpBar::CMonsterHpBar(const CMonsterHpBar& rhs)
	: CUI{ rhs }
{
}

HRESULT CMonsterHpBar::Add_Components()
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

HRESULT CMonsterHpBar::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
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


HRESULT CMonsterHpBar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterHpBar::Initialize(void* pArg)
{
	m_CurHp = ((CMonsterHPDesc*)pArg)->CurrentHp;
	m_MaxHp = ((CMonsterHPDesc*)pArg)->MaxHp;
	m_ParentMatrix = ((CMonsterHPDesc*)pArg)->ParentMat;

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	_vector vPos = XMVectorSet(m_ParentMatrix->_41, m_ParentMatrix->_42 + m_YLength, m_ParentMatrix->_43, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_PreHp = *m_CurHp;

	Set_Unique_ID("MonsterHpBar");
	return S_OK;
}

void CMonsterHpBar::Priority_Tick(_float fTimeDelta)
{

}

void CMonsterHpBar::Tick(_float fTimeDelta)
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

	_vector vPos = XMVectorSet(m_ParentMatrix->_41, m_ParentMatrix->_42 + m_YLength, m_ParentMatrix->_43, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

}

void CMonsterHpBar::Late_Tick(_float fTimeDelta)
{
	if (*CInventory::GetInstance()->Get_InvenOpen() == false)
	{
		m_pTransformCom->BillBoard();
		m_pTransformCom->Set_Scale(1.8f, 0.3f, 0.f);
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
	}
}

HRESULT CMonsterHpBar::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(2);
	
	m_pVIBufferCom->Bind_Buffers();

	
	m_pVIBufferCom->Render();

	return S_OK;
}

CMonsterHpBar* CMonsterHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterHpBar* pInstance = new CMonsterHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHpbar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterHpBar::Clone(void* pArg)
{
	CMonsterHpBar* pInstance = new CMonsterHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHpbar");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CMonsterHpBar::Free()
{
	__super::Free();

}
