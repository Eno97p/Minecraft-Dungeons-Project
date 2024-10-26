#include "Enderman.h"

#include "GameInstance.h"

CEnderman::CEnderman(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CEnderman::CEnderman(const CEnderman & rhs)
	: CMonster{ rhs }
{
}

HRESULT CEnderman::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnderman::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;


	Set_Unique_ID("Enderman");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(rand() % 5, true));

	return S_OK;
}

void CEnderman::Priority_Tick(_float fTimeDelta)
{
}

void CEnderman::Tick(_float fTimeDelta)
{
	
	m_pModelCom->Play_Animation(fTimeDelta);
	
	
}

void CEnderman::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CEnderman::Render()
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


	return S_OK;
}

HRESULT CEnderman::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enderman"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	



	return S_OK;
}

HRESULT CEnderman::Bind_ShaderResources()
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

CEnderman * CEnderman::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnderman*		pInstance = new CEnderman(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnderman::Clone(void * pArg)
{
	CEnderman*		pInstance = new CEnderman(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : Skeleton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnderman::Free()
{
	__super::Free();

}
