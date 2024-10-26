#include "stdafx.h"
#include "ParticleMesh.h"
#include "GameInstance.h"

CParticleMesh::CParticleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject(pDevice, pContext)
{
}

CParticleMesh::CParticleMesh(const CParticleMesh& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CParticleMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParticleMesh::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(nullptr)))
		return E_FAIL;

	m_eType = ((PARTICLE_DESC*)pArg)->eType;
	vStartColor = ((PARTICLE_DESC*)pArg)->vStartColor;
	vEndColor = ((PARTICLE_DESC*)pArg)->vEndColor;
	m_IsBlur = ((PARTICLE_DESC*)pArg)->IsBlur;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((PARTICLE_DESC*)pArg)->vStartPos));


	switch (((PARTICLE_DESC*)pArg)->eModelType)
	{
	case CUBE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Cube");
		break;
	case CIRCLE:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Circle");
		break;
	case FORK:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_ForkLift");
		break;
	case SLASH:
		m_ModelPrototypeTag = TEXT("Prototype_Component_Model_Slash");
		break;
	}


	if (FAILED(Add_Components(m_ModelPrototypeTag)))
		return E_FAIL;

	if (m_eType == DROP)
		m_ShaderPass = 0;
	else
		m_ShaderPass = 1;

	m_InstModelCom->Ready_Instance(((PARTICLE_DESC*)pArg)->InstanceDesc);

	Set_Unique_ID("ParticleMesh");

	return S_OK;
}

void CParticleMesh::Priority_Tick(_float fTimeDelta)
{
}

void CParticleMesh::Tick(_float fTimeDelta)
{
	if (m_InstModelCom->Check_Instance_Dead())
		m_pGameInstance->Erase(this);

	if (m_pTarget != nullptr)
	{
		CTransform* pTargetTransform = (CTransform*)m_pTarget->Get_Component(TEXT("Com_Transform"));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pTargetTransform->Get_State(CTransform::STATE_POSITION));
	}

	switch (m_eType)
	{
	case SPREAD:
		m_InstModelCom->Spread(fTimeDelta);
		break;
	case DROP:
		m_InstModelCom->Drop(fTimeDelta);
		break;
	case GROWOUT:
		m_InstModelCom->GrowOut(fTimeDelta);
		break;
	case SPREAD_SIZEUP:
		m_InstModelCom->Spread_Size_Up(fTimeDelta);
		break;
	case SPREAD_NONROTATION:
		m_InstModelCom->Spread_Non_Rotation(fTimeDelta);
		break;
	case TORNADO:
		m_InstModelCom->CreateSwirlEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN:
		m_InstModelCom->Spread_Speed_Down(fTimeDelta);
		break;
	case SLASH_EFFECT:
		m_InstModelCom->SlashEffect(fTimeDelta);
		break;
	case SPREAD_SPEED_DOWN_SIZE_UP:
		m_InstModelCom->Spread_Speed_Down_SizeUp(fTimeDelta);
		break;
	case GATHER:
		m_InstModelCom->Gather(fTimeDelta);
		break;
	case EXTINCTION:
		m_InstModelCom->Extinction(fTimeDelta);
		break;
	case GROWOUTY:
		m_InstModelCom->GrowOutY(fTimeDelta);
		break;
	}
}

void CParticleMesh::Late_Tick(_float fTimeDelta)
{
	Compute_ViewZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (m_IsBlur)
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
	else
		m_pGameInstance->Add_RenderObject(CRenderer::RENDER_BLEND, this);
}

HRESULT CParticleMesh::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint	iNumMeshes = m_InstModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_InstModelCom->Bind_Material_Instance(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		m_pShaderCom->Begin(m_ShaderPass);

		m_InstModelCom->Render_Instance(i);
	}

	return S_OK;
}

void CParticleMesh::Set_Rotaition(_float Radian, _vector Axis)
{
	m_pTransformCom->Rotation(Axis, XMConvertToRadians(Radian));
}

HRESULT CParticleMesh::Add_Components(const wstring& strModelPrototype)
{
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, strModelPrototype,
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_InstModelCom))))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxInstance_Mesh"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CParticleMesh::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	//_float3 vStartColor(1.f, 1.f, 0.f);
	//_float3 vEndColor(1.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_StartColor", &vStartColor, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_EndColor", &vEndColor, sizeof(_float3))))
		return E_FAIL;

	return S_OK;
}

CParticleMesh* CParticleMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleMesh* pInstance = new CParticleMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CParticleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CParticleMesh::Clone(void* pArg)
{
	CParticleMesh* pInstance = new CParticleMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CParticleMesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleMesh::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_InstModelCom);
}
