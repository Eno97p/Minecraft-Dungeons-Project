#include "stdafx.h"
#include "ShopNomoney.h"
#include "GameInstance.h"
#include "UIManager.h"


CShopNomoney::CShopNomoney(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShopNomoney::CShopNomoney(const CShopNomoney& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CShopNomoney::Add_Components()
{

	return S_OK;
}





HRESULT CShopNomoney::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShopNomoney::Initialize(void* pArg)
{

	m_bType = ((WARNINGDESC*)pArg)->bType;

	//if (m_bType)
	//	Sound_Yes();
	//else
	//	Sound_No();


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Shop_Nomoney");

	return S_OK;
}

void CShopNomoney::Priority_Tick(_float fTimeDelta)
{


}


void CShopNomoney::Tick(_float fTimeDelta)
{
	m_fLifeTime -= fTimeDelta;
	if (m_fLifeTime < 0.f) {
		m_fLifeTime = 0.f;
		m_pGameInstance->Erase(this);
	}
	if (S_isOpen)
	{
		m_fRatio = m_fLifeTime / 3.f;
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CShopNomoney::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SIXTH);
}

HRESULT CShopNomoney::Render()
{
	_float2 Fontpos;
	if (!m_bType)
	{
		Fontpos = { g_iWinSizeX * 0.5f - 100.f,g_iWinSizeY * 0.5f + 50.f };
		m_pGameInstance->Render_Font(TEXT("Font_Tiny"), TEXT("재화가 부족합니다."), Fontpos, XMVectorSet(1.f, 1.f, 1.f, m_fRatio));
	}
	else
	{
		Fontpos = { g_iWinSizeX * 0.5f - 70.f,g_iWinSizeY * 0.5f + 50.f };
		m_pGameInstance->Render_Font(TEXT("Font_Tiny"), TEXT("구매 완료."), Fontpos, XMVectorSet(1.f, 1.f, 1.f, m_fRatio));
	}
	return S_OK;
}

void CShopNomoney::Sound_Yes()
{


}

void CShopNomoney::Sound_No()
{

}

CShopNomoney* CShopNomoney::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShopNomoney* pInstance = new CShopNomoney(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShopNomoney");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopNomoney::Clone(void* pArg)
{
	CShopNomoney* pInstance = new CShopNomoney(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShopNomoney");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShopNomoney::Free()
{
	__super::Free();

}
