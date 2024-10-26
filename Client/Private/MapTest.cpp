#include "stdafx.h"
#include "MapTest.h"
#include "Player.h"
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
	m_pNavigationCom->Set_PickMode(true);
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
	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_SHADOWOBJ, this);
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

	/*m_pNavigationCom->Render();*/

#endif

	return S_OK;
}

HRESULT CMapTest::Render_LightDepth()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	_float4x4		ViewMatrix, ProjMatrix;

	//_vector vDir = XMVector4Normalize(XMVectorSet(-135.f, 37.f, -62.f, 1.f) - XMVectorSet(-225.f, 150.f, -135.f, 1.f));
		/* ±¤¿ø ±âÁØÀÇ ºä º¯È¯Çà·Ä. */
	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(XMVectorSet(68.f, 150.f, -126.f, 0.f), XMVectorSet(83.f, 24.f, -36.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 3000.f));

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
		return E_FAIL;

	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(4);

		m_pModelCom->Render(i);
	}

	return S_OK;
}

HRESULT CMapTest::Add_Components()
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_MapTest"),
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

