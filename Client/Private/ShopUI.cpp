#include "stdafx.h"
#include "ShopUI.h"
#include "GameInstance.h"
_bool CShopUI::S_isOpen = false;

CShopUI::CShopUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CShopUI::CShopUI(const CShopUI& rhs)
	: CUI{ rhs }
{
}


HRESULT CShopUI::Initialize(void* pArg)
{
	if (pArg != nullptr)
	{
		m_fX = ((SHOPDESC*)pArg)->In_Pos.x;
		m_fY = ((SHOPDESC*)pArg)->In_Pos.y;
		m_fSizeX = ((SHOPDESC*)pArg)->In_Size.x;
		m_fSizeY = ((SHOPDESC*)pArg)->In_Size.y;
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector		vPos = XMVectorZero();
		vPos = XMVectorSetX(vPos, -(g_iWinSizeX / 2.f) + ((SHOPDESC*)pArg)->In_Pos.x);
		vPos = XMVectorSetY(vPos, (g_iWinSizeY / 2.f) - ((SHOPDESC*)pArg)->In_Pos.y);
		vPos = XMVectorSetZ(vPos, 0.f);
		vPos = XMVectorSetW(vPos, 1.f);


		m_pTransformCom->Set_Scale(m_fSizeX, m_fSizeY, 0.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	}

	/*-(g_iWinSizeX / 2.f)*/
	//(g_iWinSizeY / 2.f)
	m_Mouse = static_cast<CUI*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Mouse"), "Mouse"));

	return S_OK;
}

void CShopUI::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	isMouseOn = CollisionRect(m_Mouse->Get_CollisionRect());
}

void CShopUI::Free()
{
	__super::Free();
}
