#include "stdafx.h"
#include "Shop_Info.h"
#include "GameInstance.h"
#include "UIManager.h"

CShop_Info::CShop_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CShopUI{ pDevice, pContext }
{
}

CShop_Info::CShop_Info(const CShop_Info& rhs)
	: CShopUI{ rhs }
{
}

HRESULT CShop_Info::Add_Components()
{
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	return S_OK;
}



void CShop_Info::Set_Info_Font()
{
	m_ItemInfo = TEXT("");
	m_ItemInfo2 = TEXT("");
	switch (m_Itemindex)
	{
	case 1:
		m_ItemInfo = TEXT("체력을 50 회복합니다.");
		break;
	case 2: //카타나
		m_ItemInfo = TEXT("이 칼날은 전투에서 승리하기 전까지");
		m_ItemInfo2 = TEXT("절대 쉬지 않을 것입니다.");
		break;
	case 3:
		m_ItemInfo = TEXT("대장장이와 병사 모두가 선호하는");
		m_ItemInfo2 = TEXT("대형 망치입니다.");
		break;
	case 4:
		m_ItemInfo = TEXT("운 좋은 대장장이의 실수로");
		m_ItemInfo2 = TEXT("두 무기가 합쳐졌습니다.");
		break;
	case 5:
		m_ItemInfo = TEXT("이 도끼는 우민왕의 무자비한");
		m_ItemInfo2 = TEXT("병사들이 선호하는 무기입니다.");
		break;
	case 6:
		m_ItemInfo = TEXT("전문적으로 제작되고 세련된");
		m_ItemInfo2 = TEXT("전쟁 무기인 도끼입니다.");
		break;
	case 7:
		m_ItemInfo = TEXT("행운의 여신의 힘이");
		m_ItemInfo2 = TEXT("깃들어 있는 창입니다.");
		break;
	case 8:
		m_ItemInfo = TEXT("갑옷에 가성비를 찾는 사람들이");
		m_ItemInfo2 = TEXT("애용하는 용병 갑옷입니다.");
		break;
	case 9:
		m_ItemInfo = TEXT("오버월드의 챔피언에게 수여합니다.");
		m_ItemInfo2 = TEXT("이 갑옷은 진정한 영웅의 표식입니다.");
		break;
	case 11:		//활
		m_ItemInfo = TEXT("단순하지만 균형 잡혀 있는 활입니다.");
		break;
	case 10:		//칼
		m_ItemInfo = TEXT("튼튼하고 믿을 수 있는 칼날입니다.");
		break;
	case 12:		//칼
		m_ItemInfo = TEXT("20개의 화살이 들어있습니다.");
		break;

	}


}




HRESULT CShop_Info::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CShop_Info::Initialize(void* pArg)
{

	m_ItemName = ((ITEMINFOINFO*)pArg)->ItemName;
	m_Itemindex = ((ITEMINFOINFO*)pArg)->iItemIndex;

	switch (((ITEMINFOINFO*)pArg)->eRarity)
	{
	case COMMON:
		NameColor = { 1.f,1.f,1.f,1.f };
		break;
	case RARE:
		NameColor = { 0.f,0.6f,0.f,1.f };
		break;
	case UNIQUE:
		NameColor = { 1.f,0.647f,0.f,1.f };
		break;
	default:
		break;
	}


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Shop_Info");
	Set_Info_Font();
	return S_OK;
}

void CShop_Info::Priority_Tick(_float fTimeDelta)
{


}


void CShop_Info::Tick(_float fTimeDelta)
{
	if (S_isOpen)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CShop_Info::Late_Tick(_float fTimeDelta)
{
	if (S_isOpen)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
}

HRESULT CShop_Info::Render()
{
	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_ItemName, _float2(700.f, 430.f), XMLoadFloat4(&NameColor));
	m_pGameInstance->Render_Font(TEXT("Font_Tiny"), m_ItemInfo, _float2(700.f, 500.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Tiny"), m_ItemInfo2, _float2(700.f, 525.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//m_ItemInfo
	return S_OK;
}

CShop_Info* CShop_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CShop_Info* pInstance = new CShop_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CShop_Info");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShop_Info::Clone(void* pArg)
{
	CShop_Info* pInstance = new CShop_Info(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CShop_Info");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CShop_Info::Free()
{
	__super::Free();

}
