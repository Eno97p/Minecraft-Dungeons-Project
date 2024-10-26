#include "HeartOfEnder.h"
#include "GameInstance.h"

CHeartOfEnder::CHeartOfEnder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMonster { pDevice, pContext }
{
}

CHeartOfEnder::CHeartOfEnder(const CHeartOfEnder & rhs)
	: CMonster{ rhs }
{
}

HRESULT CHeartOfEnder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHeartOfEnder::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;

	Set_Unique_ID("Heart_Of_Ender");
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC(rand()% 3, true));


	return S_OK;
}

void CHeartOfEnder::Priority_Tick(_float fTimeDelta)
{
}

void CHeartOfEnder::Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
}

void CHeartOfEnder::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CHeartOfEnder::Render()
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

HRESULT CHeartOfEnder::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Heart_Of_Ender"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	return S_OK;
}

HRESULT CHeartOfEnder::Bind_ShaderResources()
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

CHeartOfEnder * CHeartOfEnder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHeartOfEnder*		pInstance = new CHeartOfEnder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CHeartOfEnder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHeartOfEnder::Clone(void * pArg)
{
	CHeartOfEnder*		pInstance = new CHeartOfEnder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CHeartOfEnder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHeartOfEnder::Free()
{
	__super::Free();

}
