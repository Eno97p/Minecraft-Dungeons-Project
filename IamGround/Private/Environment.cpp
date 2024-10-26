#include "Environment.h"

#include "GameInstance.h"

HRESULT CEnvironment::Set_Update(Envi_Desc* _dec)
{
	m_EnviDec = *_dec;

	m_pTransformCom->Set_Scale(m_EnviDec.Scale, m_EnviDec.Scale, m_EnviDec.Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_EnviDec.RotationAngle);
	_vector vPos = XMLoadFloat4(&m_EnviDec.Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	return S_OK;
}

void CEnvironment::Set_Pos(_vector _pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _pos);
}

_uint CEnvironment::Get_NumAnim()
{
	return m_pModelCom->Get_NumAnim();
}

void CEnvironment::Set_AnimIndex()
{
	m_pModelCom->Set_AnimationIndex(CModel::ANIMATION_DESC((_uint)m_Animindex, true));
}

CEnvironment::CEnvironment(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEnvironment::CEnvironment(const CEnvironment & rhs)
	: CGameObject{ rhs },
	m_EnviDec{rhs.m_EnviDec }
{
}

HRESULT CEnvironment::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Scale(((Envi_Desc*)pArg)->Scale, ((Envi_Desc*)pArg)->Scale, ((Envi_Desc*)pArg)->Scale);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), ((Envi_Desc*)pArg)->RotationAngle);
	_vector vPos = XMLoadFloat4(&((Envi_Desc*)pArg)->Pos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_EnviDec = *((Envi_Desc*)pArg);

	return S_OK;
}
//
//void CEnvironment::Priority_Tick(_float fTimeDelta)
//{
//}
//
void CEnvironment::Tick(_float fTimeDelta)
{
	

	
}

void CEnvironment::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}
