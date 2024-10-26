#include "..\Public\Monster.h"

#include "GameInstance.h"

HRESULT CMonster::Set_Update(MONSTER_DEC* _dec)
{
	m_MonsterDec = *_dec;

	
	m_pTransformCom->Set_Scale(m_MonsterDec.Scale, m_MonsterDec.Scale, m_MonsterDec.Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_MonsterDec.RotationAngle));
	_vector vPos = XMLoadFloat4(&m_MonsterDec.Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CMonster::Set_Pos(_vector _pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _pos);
}

_uint CMonster::Get_NumAnim()
{
	return m_pModelCom->Get_NumAnim();
}

void CMonster::Set_AnimIndex()
{
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC((_uint)m_Animindex, true));
}

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject{ rhs },
	m_MonsterDec{rhs.m_MonsterDec }
{
}

//HRESULT CMonster::Initialize_Prototype()
//{
//	return S_OK;
//}
//
HRESULT CMonster::Initialize(void * pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(((MONSTER_DEC*)pArg)->Scale, ((MONSTER_DEC*)pArg)->Scale, ((MONSTER_DEC*)pArg)->Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), ((MONSTER_DEC*)pArg)->RotationAngle);
	_vector vPos = XMLoadFloat4(&((MONSTER_DEC*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_MonsterDec = *((MONSTER_DEC*)pArg);

	return S_OK;
}
//
//void CMonster::Priority_Tick(_float fTimeDelta)
//{
//}
//
void CMonster::Tick(_float fTimeDelta)
{
	

	
}
//
//void CMonster::Late_Tick(_float fTimeDelta)
//{
//	m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
//}
//
//HRESULT CMonster::Render()
//{
//	if (FAILED(Bind_ShaderResources()))
//		return E_FAIL;
//
//
//
//	_uint	iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//	for (size_t i = 0; i < iNumMeshes; i++)
//	{
//		m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
//
//		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
//			return E_FAIL;
//
//		m_pShaderCom->Begin(0);
//
//		m_pModelCom->Render(i);
//	}
//
//	
//
//	return S_OK;
//}

//HRESULT CMonster::Add_Components(/*const wstring& ModelTag , const wstring& ShaderTag*/)
//{
//	/* For.Com_Model */
//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
//		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
//		return E_FAIL;
//
//	/* For.Com_Shader */
//	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
//		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
//		return E_FAIL;	
//
//
//
//	return S_OK;
//}

//HRESULT CMonster::Bind_ShaderResources()
//{
//	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//		return E_FAIL;
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
//		return E_FAIL;
//	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
//		return E_FAIL;
//
//
//	return S_OK;
//}

//CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
//{
//	CMonster*		pInstance = new CMonster(pDevice, pContext);
//
//	if (FAILED(pInstance->Initialize_Prototype()))
//	{
//		MSG_BOX("Failed To Created : CMonster");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//CGameObject * CMonster::Clone(void * pArg)
//{
//	CMonster*		pInstance = new CMonster(*this);
//
//	if (FAILED(pInstance->Initialize(pArg)))
//	{
//		MSG_BOX("Failed To Cloned : CMonster");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
