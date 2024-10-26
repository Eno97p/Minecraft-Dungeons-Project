#include "stdafx.h"
#include "ItemBox.h"
#include "Item.h"
#include "GameInstance.h"
#include "ParticleManager.h"
#include "Emerald.h"

_uint CItemBox::iCount = 0;

CItemBox::CItemBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnvironment{ pDevice, pContext }
{
}

CItemBox::CItemBox(const CItemBox & rhs)
	: CEnvironment{ rhs }
{
}

HRESULT CItemBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CItemBox::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("ItemBox");

	CModel::ANIMATION_DESC desc = {0,true};
	m_pModelCom->Set_AnimationIndex(desc);
	

	for (auto& iter : m_IsOpen)
		iter = false;

	return S_OK;
}

void CItemBox::Priority_Tick(_float fTimeDelta)
{
}

void CItemBox::Tick(_float fTimeDelta)
{
	if (!m_IsOpen[0] && IsPlayerApproach(1.f))
	{
		m_IsOpen[0] = true;
		CModel::ANIMATION_DESC desc = { 1,false };
		m_pModelCom->Set_AnimationIndex(desc);
		m_pGameInstance->PlaySound_Z(_T("BoxOpen.ogg"), SOUND_EFFECT, 0.3f);
		//m_pModelCom->StopAnimation(false);
	}

	if (!m_IsOpen[1] && m_IsOpen[0] && m_pModelCom->Get_Current_Ratio() > 0.6f)
	{
		switch (iCount)
		{
		case 0:
			Create_Item(1);
			Create_Item(1);
			Create_Item(6);
			Create_Item(12);
			Create_Emerald(30);
			break;
		case 1:
			Create_Item(1);
			Create_Item(5);
			Create_Item(12);
			Create_Emerald(50);
			break;
		case 2:
			Create_Item(1);
			Create_Item(3);
			Create_Item(12);
			Create_Item(9);
			Create_Item(1);
			Create_Emerald(200);
			break;
		case 3:
			Create_Item(1);
			Create_Item(1);
			Create_Item(2);
			Create_Item(12);
			Create_Emerald(300);
			break;
		}
		iCount++;
		m_IsOpen[1] = true;
		
		CParticleManager::GetInstance()->Create_Particle(56, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
		CParticleManager::GetInstance()->Create_Particle(70, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
	}
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
}

void CItemBox::Late_Tick(_float fTimeDelta)
{
	if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CItemBox::Render()
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
#ifdef _DEBUG
	m_pColliderCom->Render();
#endif

	return S_OK;
}

HRESULT CItemBox::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	


	CBounding_AABB::AABB_DESC aabbdesc = {};
	aabbdesc.vExtents = _float3(1.f, 0.5f, 1.f);
	aabbdesc.vCenter = _float3(0.f, aabbdesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbdesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CItemBox::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", m_bIsPicked)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemBox::Create_Item(_uint _iIndex)
{
	CItem::ItemDesc itemdesc{};
	itemdesc.fData = 1;
	itemdesc.fRadius = 0.1f;
	itemdesc.RandomValue = _iIndex;
	XMStoreFloat4(&itemdesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameObject* CItem = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &itemdesc);
	m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Item"), CItem);

	return S_OK;
}

HRESULT CItemBox::Create_Emerald(_uint iNumEmerald)
{
	CEmerald::EMERALDDESC eDesc{};
	XMStoreFloat4(&eDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	CGameObject* Cemerald = nullptr;
	for (int i = 0; i < iNumEmerald; ++i)
	{
		Cemerald = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Emerald"), &eDesc);
		m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Emerald"), Cemerald);
	}
		//m_pGameInstance->Add_CloneObject(LEVEL_GAMEPLAY, TEXT("Layer_Emerald"), TEXT("Prototype_GameObject_Emerald"), &eDesc);
	return S_OK;
}

CItemBox * CItemBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CItemBox*		pInstance = new CItemBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CItemBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemBox::Clone(void * pArg)
{
	CItemBox*		pInstance = new CItemBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CItemBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemBox::Free()
{
	
	__super::Free();
}
