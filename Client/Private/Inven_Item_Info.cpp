#include "stdafx.h"
#include "Inven_Item_Info.h"
#include "GameInstance.h"
#include "UIManager.h"

CInven_ItemINFO::CInven_ItemINFO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CInven_ItemINFO::CInven_ItemINFO(const CInven_ItemINFO& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CInven_ItemINFO::Add_Components(const wchar_t* TextureTag)
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

HRESULT CInven_ItemINFO::Bind_ShaderResources()
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

void CInven_ItemINFO::Set_Info_Font()
{
	m_ItemInfo = TEXT("");
	m_ItemInfo2 = TEXT("");
	switch (m_Itemindex)
	{
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
	}


}




HRESULT CInven_ItemINFO::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInven_ItemINFO::Initialize(void* pArg)
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

	if (FAILED(Add_Components(((ITEMINFOINFO*)pArg)->TextureTag)))
		return E_FAIL;

	Set_Unique_ID("Inven_ItemInfo");
	Set_Info_Font();
	return S_OK;
}

void CInven_ItemINFO::Priority_Tick(_float fTimeDelta)
{


}


void CInven_ItemINFO::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		__super::Tick(fTimeDelta);
	}
	else
		return;
}


void CInven_ItemINFO::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::SECOND);
}

HRESULT CInven_ItemINFO::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);
	
	m_pVIBufferCom->Bind_Buffers();

	m_pVIBufferCom->Render();

	m_pGameInstance->Render_Font(TEXT("Font_Default"), m_ItemName, _float2(780.f, 130.f), XMLoadFloat4(&NameColor));
	m_pGameInstance->Render_Font(TEXT("Font_Tiny"), m_ItemInfo, _float2(780.f, 200.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pGameInstance->Render_Font(TEXT("Font_Tiny"), m_ItemInfo2, _float2(780.f, 225.f), XMVectorSet(1.f, 1.f, 1.f, 1.f));
	//m_ItemInfo
	return S_OK;
}

CInven_ItemINFO* CInven_ItemINFO::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInven_ItemINFO* pInstance = new CInven_ItemINFO(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CInven_ItemINFO");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInven_ItemINFO::Clone(void* pArg)
{
	CInven_ItemINFO* pInstance = new CInven_ItemINFO(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInven_ItemINFO");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CInven_ItemINFO::Free()
{
	__super::Free();

}
