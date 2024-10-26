#include "stdafx.h"
#include "Emerald.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"
#include "ParticleManager.h"

CEmerald::CEmerald(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEmerald::CEmerald(const CEmerald& rhs)
    : CGameObject{ rhs }
{

}


HRESULT CEmerald::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEmerald::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));
    Description = *(reinterpret_cast<EMERALDDESC*>(pArg));
    
    ((EMERALDDESC*)pArg)->fSpeedPerSec = RandomFloat(1.f, 3.f);
    ((EMERALDDESC*)pArg)->fRotationPerSec = XMConvertToRadians(RandomFloat(480.f, 600.f));

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scale(1.f, 1.f, 1.f);
    Description.vPos.y += 0.4f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Description.vPos));

    Description.vDir = { RandomFloat(0.f,360.f) * RandomSign(),0.f, RandomFloat(0.f,360.f) * RandomSign() };


    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Power(RandomFloat(3.f, 7.f));
    m_pTransformCom->Set_JumpState(true);
    m_pGameInstance->PlaySound_Z(TEXT("sfx_item_emeraldBurstOutPing-003_soundWave.ogg"), SOUND_EFFECT, 0.2f);
    return S_OK;
}

void CEmerald::Priority_Tick(_float fTimeDelta)
{
}

void CEmerald::Tick(_float fTimeDelta)
{
    m_pTransformCom->Turn_Slowly(XMVectorSet(0.f,1.f,0.f,0.f), fTimeDelta);
    if (m_pTransformCom->isJumping())
    {
        _float4 a;
        XMStoreFloat4(&a, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        m_pTransformCom->Jump(fTimeDelta, m_pNavigationCom);
        m_pTransformCom->MoveDirection(XMVector3Normalize(XMLoadFloat3(&Description.vDir)),fTimeDelta, 3.f, m_pNavigationCom);
        if (Description.vPos.y > a.y)
        {
            m_pGameInstance->PlaySound_Z(TEXT("sfx_item_emerald_rightcollect-001.ogg"), SOUND_EFFECT, 0.2f);
            m_pTransformCom->Set_JumpState(false);
        }
    }
    else
    {

        if (IsPlayerApproach(6.f))
        {
            m_pTransformCom->Moving_Lerp(fTimeDelta, Get_PlayerPos());
            if (IsPlayerApproach(1.f))
            {
                CInventory::GetInstance()->Add_Emerald();
                m_pGameInstance->PlaySound_Z(TEXT("sfx_item_emeraldCollect-002_soundWave.ogg"), SOUND_EFFECT, 0.4f);
                _vector Dst = Get_PlayerPos();
                XMVectorSetY(Dst, XMVectorGetY(Dst) + 0.5f);
                CParticleManager::GetInstance()->Create_Particle(71, Dst, nullptr);
                m_pGameInstance->Erase(this);
            }

        }

    }
}

void CEmerald::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CEmerald::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint	iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_Texture", i, aiTextureType_DIFFUSE)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }


    return S_OK;
}



HRESULT CEmerald::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Emerald"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;


    CNavigation::NAVIGATION_DESC	NavigationDesc{};

    NavigationDesc.iCurrentCellIndex = 0;
    NavigationDesc.vPos = &Description.vPos;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CEmerald::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Bool("g_Picked", false)))
        return E_FAIL;

    return S_OK;
}

_bool CEmerald::IsPlayerApproach(_float fDist)
{
    if (XMVectorGetX(XMVector4Length(Get_PlayerPos() -
        m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < fDist)
        return true;
    else
        return false;
}

_vector CEmerald::Get_PlayerPos()
{
    CTransform* PlayerTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
    return PlayerTransform->Get_State(CTransform::STATE_POSITION);
}


CEmerald* CEmerald::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEmerald* pInstance = new CEmerald(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CEmerald");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEmerald::Clone(void* pArg)
{
    CEmerald* pInstance = new CEmerald(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : Circle");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEmerald::Free()
{
    __super::Free();
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pNavigationCom);
}
