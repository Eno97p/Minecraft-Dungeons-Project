#include "MapTest.h"

#include "GameInstance.h"

CMapTest::CMapTest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CMap { pDevice, pContext }
{
}

CMapTest::CMapTest(const CMapTest & rhs)
	: CMap{ rhs }
{
}

HRESULT CMapTest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapTest::Initialize(void * pArg)
{

	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(Add_Components()))
		return E_FAIL;
	m_pTransformCom->Set_Scale(((MAP_DEC*)pArg)->Scale.x, ((MAP_DEC*)pArg)->Scale.y, ((MAP_DEC*)pArg)->Scale.z);
	_vector vPos = XMLoadFloat4(&((MAP_DEC*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_Mapdec = *(MAP_DEC*)pArg;
	Set_Unique_ID("Map");
	return S_OK;
}

void CMapTest::Priority_Tick(_float fTimeDelta)
{
}

void CMapTest::Tick(_float fTimeDelta)
{
	/*m_pModelCom->Play_Animation(fTimeDelta);*/
	
	
}

void CMapTest::Late_Tick(_float fTimeDelta)
{
	m_pNavigationCom->Update(m_pTransformCom);
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMapTest::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		/*m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);*/

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	
#ifdef _DEBUG	
	m_pGameInstance->Add_DebugComponent(m_pNavigationCom);
#endif


	return S_OK;
}



HRESULT CMapTest::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FirstStage"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;	

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
		TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapTest::Bind_ShaderResources()
{

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CMapTest * CMapTest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapTest*		pInstance = new CMapTest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : MapTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapTest::Clone(void * pArg)
{
	CMapTest*		pInstance = new CMapTest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : MapTest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapTest::Free()
{
	__super::Free();

}
