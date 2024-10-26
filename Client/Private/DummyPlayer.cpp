#include "stdafx.h"
#include "DummyPlayer.h"
#include "GameInstance.h"
#include "UIManager.h"
#include "Armor.h"
#include "Weapon.h"


CDummyPlayer::CDummyPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CInvenUI{ pDevice, pContext }
{
}

CDummyPlayer::CDummyPlayer(const CDummyPlayer& rhs)
	: CInvenUI{ rhs }
{
}

HRESULT CDummyPlayer::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh_Old"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix",&m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDummyPlayer::Add_PartObjects()
{
	CGameObject* pWeapon = nullptr;

	m_Weapons.reserve(CPlayer::WEAPON_END);

	CWeapon::WEAPON_DESC			WeaponDesc{};
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	/*WeaponDesc.pState = &m_iCurrentState;*/



	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;

	//Add Sword
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Sword"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Bow
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_L_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;


	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Bow"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add DoubleAxe
	WeaponDesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_R_Weapon");
	if (nullptr == WeaponDesc.pCombinedTransformationMatrix)
		return E_FAIL;


	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_DoubleAxe"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Spear
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Spear"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Axe
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Axe"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Katana
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Katana"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Hammer
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hammer"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	//Add Claymore
	pWeapon = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Claymore"), &WeaponDesc);
	if (nullptr == pWeapon)
		return E_FAIL;

	m_Weapons.emplace_back(pWeapon);

	CGameObject* pArmor = nullptr;

	CArmor::ARMORDESC armordesc{};
	armordesc.pParentMatrix = m_pTransformCom->Get_WorldFloat4x4();
	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Body_armor");

	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_Body"), &armordesc);
	m_Armors[CPlayer::MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_Head");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_Head"), &armordesc);
	m_Armors[CPlayer::MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("L_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_L_Arm"), &armordesc);
	m_Armors[CPlayer::MERCENARY].emplace_back(pArmor);

	/*armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("L_Leg_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_L_Leg"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = pModelCom->Get_BoneCombinedTransformationMatrix("R_Leg_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_R_Leg"), &armordesc);
	m_Armors[MERCENARY].emplace_back(pArmor);*/

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("R_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_MA_R_Arm"), &armordesc);
	m_Armors[CPlayer::MERCENARY].emplace_back(pArmor);

	/* ==============champions armor =============*/

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("Body_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_Body"), &armordesc);
	m_Armors[CPlayer::CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("J_Head");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_Head"), &armordesc);
	m_Armors[CPlayer::CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("L_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_L_Arm"), &armordesc);
	m_Armors[CPlayer::CHAMPION].emplace_back(pArmor);

	armordesc.pCombinedTransformationMatrix = m_pModelCom->Get_BoneCombinedTransformationMatrix("R_Arm_armor");
	pArmor = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Cham_R_Arm"), &armordesc);
	m_Armors[CPlayer::CHAMPION].emplace_back(pArmor);

	return S_OK;
}




HRESULT CDummyPlayer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummyPlayer::Initialize(void* pArg)
{

	m_fX = (g_iWinSizeX >> 1) - 0.65f;
	m_fY = (g_iWinSizeY >> 1) + 0.55f;


	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(18, true));


	_vector		vPos = XMVectorZero();
	vPos = XMVectorSetX(vPos, -(g_iWinSizeX / 2.f) + m_fX);
	vPos = XMVectorSetY(vPos, (g_iWinSizeY / 2.f) - m_fY);
	vPos = XMVectorSetZ(vPos, 0.5f);
	vPos = XMVectorSetW(vPos, 1.f);


	m_pTransformCom->Set_Scale(0.5f, 0.9f, 0.5f);
	m_pTransformCom->Rotation_Z(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_ViewMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW);
	m_ProjMatrix = *m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ);
	if (FAILED(Add_PartObjects()))
		return E_FAIL;

	Set_Unique_ID("DummyPlayer");
	return S_OK;
}

void CDummyPlayer::Priority_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		/*m_Weapons[*m_iCurrentWeapon]->Priority_Tick(fTimeDelta);
		if (*m_iCurrentArmor != 0)
		{
			for (auto& pPartArmor : m_Armors[*m_iCurrentArmor])
				pPartArmor->Priority_Tick(fTimeDelta);
		}*/
	}
}


void CDummyPlayer::Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		CModel::ANIMATION_DESC		AnimDesc{ 26, true };
		m_pModelCom->Set_AnimationIndex(AnimDesc);
		m_pModelCom->Play_Animation(fTimeDelta * 0.5f);

		/*if (*m_iCurrentArmor != 0)
		{
			for (auto& pPartArmor : m_Armors[*m_iCurrentArmor])
				pPartArmor->Tick(fTimeDelta);
		}
		m_Weapons[*m_iCurrentWeapon]->Tick(fTimeDelta);*/
	}
}


void CDummyPlayer::Late_Tick(_float fTimeDelta)
{
	if (isOpen && !AllClose)
	{
		CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::THIRD);
	}
}

HRESULT CDummyPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	if (*m_iCurrentArmor != 0)
	{
		for (auto& pPartArmor : m_Armors[*m_iCurrentArmor])
		{
			static_cast<CArmor*>(pPartArmor)->Socket_Matrix_Bind();
			static_cast<CArmor*>(pPartArmor)->Bind_ProjResources(&m_ViewMatrix, &m_ProjMatrix);
			static_cast<CArmor*>(pPartArmor)->RenderUI();
		}
	}

	static_cast<CWeapon*>(m_Weapons[*m_iCurrentWeapon])->Socket_Matrix_Bind();
	static_cast<CWeapon*>(m_Weapons[*m_iCurrentWeapon])->Bind_ProjResources(&m_ViewMatrix, &m_ProjMatrix);
	static_cast<CWeapon*>(m_Weapons[*m_iCurrentWeapon])->RenderUI();

	return S_OK;
}

void CDummyPlayer::SetPlayer(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
	m_iCurrentWeapon = m_pPlayer->Get_CurWeapon();
	m_iCurrentArmor = m_pPlayer->Get_CurAmo();
}

CDummyPlayer* CDummyPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummyPlayer* pInstance = new CDummyPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CDummyPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummyPlayer::Clone(void* pArg)
{
	CDummyPlayer* pInstance = new CDummyPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CDummyPlayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}
void CDummyPlayer::Free()
{
	for (auto& pPartWeapon : m_Weapons)
		Safe_Release(pPartWeapon);

	for (auto& pPartArmor : m_Armors[CPlayer::MERCENARY])
		Safe_Release(pPartArmor);
	for (auto& pPartArmor : m_Armors[CPlayer::CHAMPION])
		Safe_Release(pPartArmor);
	Safe_Release(m_pModelCom);
	__super::Free();

}
