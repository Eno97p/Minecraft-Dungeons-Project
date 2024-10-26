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
		m_ItemInfo = TEXT("ü���� 50 ȸ���մϴ�.");
		break;
	case 2: //īŸ��
		m_ItemInfo = TEXT("�� Į���� �������� �¸��ϱ� ������");
		m_ItemInfo2 = TEXT("���� ���� ���� ���Դϴ�.");
		break;
	case 3:
		m_ItemInfo = TEXT("�������̿� ���� ��ΰ� ��ȣ�ϴ�");
		m_ItemInfo2 = TEXT("���� ��ġ�Դϴ�.");
		break;
	case 4:
		m_ItemInfo = TEXT("�� ���� ���������� �Ǽ���");
		m_ItemInfo2 = TEXT("�� ���Ⱑ ���������ϴ�.");
		break;
	case 5:
		m_ItemInfo = TEXT("�� ������ ��ο��� ���ں���");
		m_ItemInfo2 = TEXT("������� ��ȣ�ϴ� �����Դϴ�.");
		break;
	case 6:
		m_ItemInfo = TEXT("���������� ���۵ǰ� ���õ�");
		m_ItemInfo2 = TEXT("���� ������ �����Դϴ�.");
		break;
	case 7:
		m_ItemInfo = TEXT("����� ������ ����");
		m_ItemInfo2 = TEXT("���� �ִ� â�Դϴ�.");
		break;
	case 8:
		m_ItemInfo = TEXT("���ʿ� ������ ã�� �������");
		m_ItemInfo2 = TEXT("�ֿ��ϴ� �뺴 �����Դϴ�.");
		break;
	case 9:
		m_ItemInfo = TEXT("���������� è�Ǿ𿡰� �����մϴ�.");
		m_ItemInfo2 = TEXT("�� ������ ������ ������ ǥ���Դϴ�.");
		break;
	case 11:		//Ȱ
		m_ItemInfo = TEXT("�ܼ������� ���� ���� �ִ� Ȱ�Դϴ�.");
		break;
	case 10:		//Į
		m_ItemInfo = TEXT("ưư�ϰ� ���� �� �ִ� Į���Դϴ�.");
		break;
	case 12:		//Į
		m_ItemInfo = TEXT("20���� ȭ���� ����ֽ��ϴ�.");
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
