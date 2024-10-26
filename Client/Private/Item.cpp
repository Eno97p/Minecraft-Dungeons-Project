#include "stdafx.h"
#include "Item.h"
#include "GameInstance.h"
#include "Inventory.h"
#include "Player.h"
#include "ParticleManager.h"
#include "EpicBeam.h"
#include "EpicGround.h"
#include "EpicPillar.h"
CItem::CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CItem::CItem(const CItem& rhs)
    : CGameObject{ rhs }
{

}


HRESULT CItem::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CItem::Initialize(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));
    Description = *(reinterpret_cast<ItemDesc*>(pArg));

    Set_Info();
    

    ((ItemDesc*)pArg)->fSpeedPerSec = RandomFloat(1.f, 3.f);
    ((ItemDesc*)pArg)->fRotationPerSec = XMConvertToRadians(RandomFloat(30.f, 90.f));

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_Scale(Description.fRadius, Description.fRadius, 0.f);
    Description.vPos.y += 0.4f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Description.vPos));

    Description.vDir = { RandomFloat(0.f,360.f) * RandomSign(),0.f, RandomFloat(0.f,360.f) * RandomSign() };



    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pTransformCom->Set_Power(RandomFloat(3.f, 7.f));
    m_pTransformCom->Set_JumpState(true);

    Isdead = new _bool;
    *Isdead = false;

    if (Description.RandomValue == 2 || Description.RandomValue == 9)
        CreateEpicBeam();


    

    return S_OK;
}

void CItem::Priority_Tick(_float fTimeDelta)
{
}

void CItem::Tick(_float fTimeDelta)
{
    if (m_pTransformCom->isJumping())
    {
        _float4 a;
        XMStoreFloat4(&a, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        m_pTransformCom->Jump(fTimeDelta, m_pNavigationCom);
        m_pTransformCom->MoveDirection(XMVector3Normalize(XMLoadFloat3(&Description.vDir)),fTimeDelta, 3.f, m_pNavigationCom);
        if (Description.vPos.y > a.y)
            m_pTransformCom->Set_JumpState(false);
    }
    else
    {

        if (IsPlayerApproach(3.f))
        {
            m_pTransformCom->Moving_Lerp(fTimeDelta, Get_PlayerPos());
            if (IsPlayerApproach(0.5f))
            {
                CInventory::GetInstance()->Add_Inven_Item(this);
                *Isdead = true;
                m_pGameInstance->Erase(this);
            }
        }

    }

 



}

void CItem::Late_Tick(_float fTimeDelta)
{
    if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
    {
        m_pTransformCom->BillBoard();
        m_pGameInstance->Add_RenderObject(CRenderer::RENDER_NONBLEND, this);
        ItemParticle();
    }
}

HRESULT CItem::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();

#ifdef _DEBUG


#endif

    return S_OK;
}

void CItem::Set_Info()
{
    switch (Description.RandomValue)
    {
    case 0:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Swift_Potion");
        Description.Item_Name = TEXT("Swift Potion");
        break;
    case 1:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Potion");
        Description.Item_Name = TEXT("Potion");
        break;
    case 2:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Katana");
        Description.Item_Name = TEXT("Katana");
        m_pGameInstance->PlaySound_Z(_T("sfx_item_championsArmorUnique1Land-001_soundWave.ogg"), SOUND_EFFECT, 0.4f);
        break;
    case 3:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Hammer");
        Description.Item_Name = TEXT("Hammer");
        break;
    case 4:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Claymore");
        Description.Item_Name = TEXT("Claymore");
        break;
    case 5:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_DoubleAxe");
        Description.Item_Name = TEXT("One Handed Ax");
        break;
    case 6:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Axe");
        Description.Item_Name = TEXT("Axe");
        break;
    case 7:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Spear");
        Description.Item_Name = TEXT("Spear");
        m_pGameInstance->PlaySound_Z(_T("sfx_item_championsArmorUnique1Land-001_soundWave.ogg"), SOUND_EFFECT, 0.4f);
        
        break;
    case 8:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_MercenaryArmor");
        Description.Item_Name = TEXT("Mercenary Armor");
        break;
    case 9:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_ChampionArmor");
        Description.Item_Name = TEXT("Champions Armor");
        m_pGameInstance->PlaySound_Z(_T("sfx_item_championsArmorUnique1Land-001_soundWave.ogg"), SOUND_EFFECT, 0.4f);
        break;
    case 10:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Sword");
        Description.Item_Name = TEXT("Sword");
        break;
    case 11:
        Description.TextureTag = TEXT("Prototype_Component_Texture_Item_Bow");
        Description.Item_Name = TEXT("Bow");
        break;
    case 12:
        Description.TextureTag = TEXT("Prototype_Component_Texture_ArrowIcon");
        Description.Item_Name = TEXT("Arrows");
        break;
    }
    

  
}

void CItem::CreateEpicBeam()
{
    m_pGameInstance->PlaySound_Z(_T("legendary_epic_item_fall_02.ogg"), SOUND_EFFECT, 0.2f);

    CGameObject* EpicBeam = nullptr;
    CEpicBeam::EpicDesc desc{};
    desc.WorldTrans = m_pTransformCom->Get_WorldFloat4x4();
    EpicBeam = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_EpicBeam"), &desc);
    m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("EpicBeam"), EpicBeam);
   

    CEpicGround::EpicGroundDesc gdesc{};
    gdesc.WorldTrans = m_pTransformCom->Get_WorldFloat4x4();
    gdesc.Isdead = Isdead;
    EpicBeam = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_EpicGround"), &gdesc);
    m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("EpicBeam"), EpicBeam);


    //CEpicPillar::EpicPillarDesc pDesc{};
    //pDesc.WorldTrans = m_pTransformCom->Get_WorldFloat4x4();
    //pDesc.Isdead = Isdead;
    //EpicBeam = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_EpicPillar"), &pDesc);
    //m_pGameInstance->CreateObject(LEVEL_GAMEPLAY, TEXT("EpicBeam"), EpicBeam);


}

HRESULT CItem::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Bill"), //°íÃÄ¾ß´ï
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, Description.TextureTag,
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    CNavigation::NAVIGATION_DESC	NavigationDesc{};

    NavigationDesc.iCurrentCellIndex = 0;
    NavigationDesc.vPos = &Description.vPos;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NavigationDesc)))
        return E_FAIL;



    return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition_float4(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

_bool CItem::IsPlayerApproach(_float fDist)
{
    if (XMVectorGetX(XMVector4Length(Get_PlayerPos() -
        m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < fDist)
        return true;
    else
        return false;
}

_vector CItem::Get_PlayerPos()
{
    CTransform* PlayerTransform = static_cast<CTransform*>(pPlayer->Get_Component(TEXT("Com_Transform")));
    return PlayerTransform->Get_State(CTransform::STATE_POSITION);
}

void CItem::ItemParticle()
{
    switch (Description.RandomValue)
    {
    case 2:
        CParticleManager::GetInstance()->Create_Particle(59, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 3:
        CParticleManager::GetInstance()->Create_Particle(58, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 4:
        CParticleManager::GetInstance()->Create_Particle(58, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 5:
        CParticleManager::GetInstance()->Create_Particle(57, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 6:
        CParticleManager::GetInstance()->Create_Particle(57, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 7:
        CParticleManager::GetInstance()->Create_Particle(59, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 8:
        CParticleManager::GetInstance()->Create_Particle(58, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    case 9:
        CParticleManager::GetInstance()->Create_Particle(59, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        break;
    default:
        break;
    }
}


CItem* CItem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CItem* pInstance = new CItem(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CItem");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
    CItem* pInstance = new CItem(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : Circle");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CItem::Free()
{
    if (IsCloned == true)
        Safe_Delete(Isdead);

    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pNavigationCom);
}
