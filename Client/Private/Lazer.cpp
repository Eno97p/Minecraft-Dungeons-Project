#include "stdafx.h"
#include "Lazer.h"
#include "GameInstance.h"
#include "ParticleManager.h"
#include "Player.h"

CLazer::CLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CLazer::CLazer(const CLazer& rhs)
    : CGameObject{ rhs }
{

}

HRESULT CLazer::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CLazer::Initialize(void* pArg)
{

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_ZSize = ((LAZERDESC*)pArg)->vZSize;
    m_OriginZsize = m_ZSize;
    if (FAILED(Add_Components()))
        return E_FAIL;

    


    m_ParentMatrix = ((LAZERDESC*)pArg)->ParentMatrix;
    m_LifeTime = ((LAZERDESC*)pArg)->LifeTime;
    m_eDirType = ((LAZERDESC*)pArg)->eDirType;
    m_pTransformCom->Rotation_Z(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));

    XMVECTOR vLook = XMVector3Normalize(XMVectorSet(m_ParentMatrix->_31, m_ParentMatrix->_32, m_ParentMatrix->_33, 0.f));
    XMVECTOR vRight = XMVector3Normalize(XMVectorSet(m_ParentMatrix->_11, m_ParentMatrix->_12, m_ParentMatrix->_13, 0.f));
    XMVECTOR vUp = XMVector3Normalize(XMVectorSet(m_ParentMatrix->_21, m_ParentMatrix->_22, m_ParentMatrix->_23, 0.f));
    XMVECTOR vLeft = vRight * -1.f;
    XMVECTOR vBack = vLook * -1.f;

    switch (m_eDirType)
    {
    case LOOK:
        break;
    case RIGHT_LOOK:
        vLook = XMVector3Normalize(XMVectorLerp(vLook, vRight, 0.5f));
        vRight = XMVector3Cross(vLook,vUp);
        break;
    case RIGHT_LOOKMINUS:
        vLook = XMVector3Normalize(XMVectorLerp(vBack, vRight, 0.5f));
        vRight = XMVector3Cross(vLook, vUp);
        break;
    case LEFT_LOOK:
        vLook = XMVector3Normalize(XMVectorLerp(vLook, vLeft, 0.5f));
        vRight = XMVector3Cross(vLook, vUp);
        break;
    case LEFT_LOOKMINUS:
        vLook = XMVector3Normalize(XMVectorLerp(vBack, vLeft, 0.5f));
        vRight = XMVector3Cross(vLook, vUp);
        break;
    }

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

    _vector Pos = XMVectorSet(m_ParentMatrix->_41, m_ParentMatrix->_42+0.5f, m_ParentMatrix->_43, 1.f);
   
    m_pTransformCom->Rotation_Z(vLook, XMConvertToRadians(((LAZERDESC*)pArg)->fStartRotation));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, (Pos + (vLook * m_ZSize)));

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));
    m_pPlayerCollider = static_cast<CCollider*>(m_pPlayer->Get_Component(TEXT("Com_Collider")));
    
    return S_OK;
}

void CLazer::Priority_Tick(_float fTimeDelta)
{

}

void CLazer::Tick(_float fTimeDelta)
{
    m_LifeTime -= fTimeDelta;
    if (m_LifeTime < 0.f)
        LazerDead = true;


    if (!LazerDead)
    {
        m_XSize += fTimeDelta;
        if (m_XSize > m_MaxSize)
            m_XSize = m_MaxSize;
    }
    else
    {
        m_XSize -= fTimeDelta;
        if (m_XSize < 0.f)
        {
            m_XSize = 0.f;
            m_pGameInstance->Erase(this);
        }
    }
   
    Direction_Calculator(fTimeDelta);

}

void CLazer::Late_Tick(_float fTimeDelta)
{
    //m_pTransformCom->BillBoard();
    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);


}

HRESULT CLazer::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }




    return S_OK;
}

void CLazer::Direction_Calculator(_float fTimeDelta)
{
    XMVECTOR vLook = XMVector3Normalize(XMVectorSet(m_ParentMatrix->_31, m_ParentMatrix->_32, m_ParentMatrix->_33, 0.f));

    XMVECTOR vRight = XMVector3Normalize(XMVectorSet(m_ParentMatrix->_11, m_ParentMatrix->_12, m_ParentMatrix->_13, 0.f));
    XMVECTOR vLeft = vRight * -1.f;
    XMVECTOR vBack = vLook * -1.f;
    XMVECTOR parentPos = XMVectorSet(m_ParentMatrix->_41, m_ParentMatrix->_42 + 0.5f, m_ParentMatrix->_43, 1.f);

    XMVECTOR vRightFront = XMVector3Normalize(XMVectorLerp(vLook, vRight, 0.5f));
    XMVECTOR vRightBack = XMVector3Normalize(XMVectorLerp(vBack, vRight, 0.5f));
    XMVECTOR vLeftFront = XMVector3Normalize(XMVectorLerp(vLook, vLeft, 0.5f));
    XMVECTOR vLeftBack = XMVector3Normalize(XMVectorLerp(vBack, vLeft, 0.5f));

    XMVECTOR childPos = XMVectorZero();
    XMVECTOR LazerEndPos= XMVectorZero();

    m_pTransformCom->TurnQuarternion(m_pTransformCom->Get_State(CTransform::STATE_LOOK), fTimeDelta);

    switch (m_eDirType)
    {
    case LOOK:
        childPos = parentPos + vLook * (m_ZSize);
        LazerEndPos = parentPos + vLook * (m_ZSize * 2.f);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
        break;
    case RIGHT_LOOK:
        childPos = parentPos + vRightFront * (m_ZSize);
        LazerEndPos = parentPos + vRightFront * (m_ZSize * 2.f);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vRightFront);
        break;
    case RIGHT_LOOKMINUS:
        childPos = parentPos + vRightBack * (m_ZSize);
        LazerEndPos = parentPos + vRightBack * (m_ZSize * 2.f);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vRightBack);
        break;
    case LEFT_LOOK:
        childPos = parentPos + vLeftFront * (m_ZSize);
        LazerEndPos = parentPos + vLeftFront * (m_ZSize * 2.f);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLeftFront);
        break;
    case LEFT_LOOKMINUS:
        childPos = parentPos + vLeftBack * (m_ZSize);
        LazerEndPos = parentPos + vLeftBack * (m_ZSize * 2.f);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLeftBack);
        break;
    }

    _float fDist = m_ZSize;
    if (m_pPlayerCollider->IntersectRay(parentPos, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)), fDist)
        && !LazerDead)
    {
        if (XMVectorGetX(XMVector3Length(m_pPlayer->Get_Pos() - parentPos)) <= XMVectorGetX(XMVector3Length(LazerEndPos - parentPos)))
        {
            m_pPlayer->Set_Damage(0.2f);
            m_ZSize = XMVectorGetX(XMVector3Length(m_pPlayer->Get_Pos() - parentPos)) *0.5f;
            LazerEndPos = m_pPlayer->Get_Pos();
        }

        if(m_ZSize != m_OriginZsize)
            m_pPlayer->Set_Damage(0.2f);
       
    }
    else
    {
        m_ZSize = m_OriginZsize;
        switch (m_eDirType)
        {
        case LOOK:
            childPos = parentPos + vLook * (m_ZSize);
            break;
        case RIGHT_LOOK:
            childPos = parentPos + vRightFront * (m_ZSize);
            break;
        case RIGHT_LOOKMINUS:
            childPos = parentPos + vRightBack * (m_ZSize);
            break;
        case LEFT_LOOK:
            childPos = parentPos + vLeftFront * (m_ZSize);
            break;
        case LEFT_LOOKMINUS:
            childPos = parentPos + vLeftBack * (m_ZSize);
            break;
        }
       
    }
        

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, childPos);
    m_pTransformCom->Set_Scale(m_XSize, 0.f, m_ZSize);

    for (int i = 0; i < 2; ++i)
    {
        CParticleManager::GetInstance()->Create_Particle(39, LazerEndPos, nullptr);
        CParticleManager::GetInstance()->Create_Particle(40, LazerEndPos, nullptr);
    }
    
}



HRESULT CLazer::Add_Components()
{
   
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Lazer"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_CubeTexture"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    return S_OK;
}

HRESULT CLazer::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    return S_OK;
}

CLazer* CLazer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLazer* pInstance = new CLazer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : Lazer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLazer::Clone(void* pArg)
{
    CLazer* pInstance = new CLazer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : Lazer");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLazer::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);


}
