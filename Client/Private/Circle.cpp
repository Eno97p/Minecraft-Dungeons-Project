#include "stdafx.h"
#include "Circle.h"
#include "GameInstance.h"
#include "ParticleManager.h"
#include "Player.h"
#include "UIManager.h"

CCircle::CCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBlendObject{ pDevice, pContext }
{
}

CCircle::CCircle(const CCircle& rhs)
    : CBlendObject{ rhs }
{

}

HRESULT CCircle::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCircle::Initialize(void* pArg)
{
  
    
    if (FAILED(__super::Initialize(nullptr)))
        return E_FAIL;

    m_fLifetimeMax = ((CIRCLEDESC*)pArg)->LifeTimeMax;
    m_fRadius = ((CIRCLEDESC*)pArg)->fRadius;
    m_f4Pos = _float3(((CIRCLEDESC*)pArg)->vPos.x, ((CIRCLEDESC*)pArg)->vPos.y, ((CIRCLEDESC*)pArg)->vPos.z);
    m_ParticleNum = ((CIRCLEDESC*)pArg)->ParticleNum;

    m_pTransformCom->Set_Scale(m_fRadius, m_fRadius, 0.1f);
    m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&((CIRCLEDESC*)pArg)->vPos));
    

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Find_Object_By_ID(LEVEL_GAMEPLAY, TEXT("Layer_Player"), "Player"));


    return S_OK;
}

void CCircle::Priority_Tick(_float fTimeDelta)
{

}

void CCircle::Tick(_float fTimeDelta)
{
    if (m_fLifetime >= m_fLifetimeMax *0.5f)
    {
        MakeParticle();
        
        m_pGameInstance->Erase(this);
        return;
    }

    m_fLifetime += fTimeDelta;
    m_fRatio = m_fLifetime / m_fLifetimeMax;
    m_fRatio = m_fRatio > 1.0f ? 1.0f : (m_fRatio < 0.0f ? 0.0f : m_fRatio);

 
  /*  list<class CGameObject*> monsters = m_pGameInstance->Get_Layer_Objects(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
    for (auto& iter : monsters)
    {
        if (m_pColliderCom->Intersect(static_cast<CCollider*>(iter->Get_Component(TEXT("Com_Collider")))))
            int a = 0;
    }*/
    

    
   
   
    
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

}

void CCircle::Late_Tick(_float fTimeDelta)
{
    if (true == m_pGameInstance->isIn_WorldFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f))
    {
        CUIManager::GetInstance()->Add_RenderUI(this, CUIManager::FIRST);
       // m_pGameInstance->Add_RenderObject(CRenderer::RENDER_UI, this);
    }
    
}

HRESULT CCircle::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_Buffers();

    m_pVIBufferCom->Render();


#ifdef _DEBUG
    
    m_pColliderCom->Render();
#endif 

   

    return S_OK;
}



void CCircle::MakeParticle()
{
    switch (m_ParticleNum)
    {
    case 0: //Slam
        CParticleManager::GetInstance()->Create_Particle(2, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(3, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        if (InterSected(m_pPlayer))
        {
            m_pPlayer->Set_Hit(true, true);
            m_pPlayer->Set_Damage(30.f);
        }
        ExplosionSound();
        break;
    case 1: //ÀÛÀºÀåÆÇ
        CParticleManager::GetInstance()->Create_Particle(8, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(9, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(12, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(13, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(37, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        if (InterSected(m_pPlayer))
        {
            m_pPlayer->Set_Hit(true, true);
            m_pPlayer->Set_Damage(25.f);
        }
        ExplosionSound();
        break;
    case 2: //ÀÛÀº°ñ·½°Å
        CParticleManager::GetInstance()->Create_Particle(61, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        CParticleManager::GetInstance()->Create_Particle(62, m_pTransformCom->Get_State(CTransform::STATE_POSITION), nullptr);
        if (InterSected(m_pPlayer))
        {
            m_pPlayer->Set_Hit(true, true);
            m_pPlayer->Set_Damage(15.f);
        }
        SmallExplosion();
        break;
    }

   
}

void CCircle::ExplosionSound()
{
    _int RandomSound = RandomInt(0, 3);
    switch (RandomSound)
    {
    case 0:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    case 1:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-002_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    case 2:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-003_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    case 3:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_explode-004_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    }


}

void CCircle::SmallExplosion()
{
    _int RandomSound = RandomInt(0, 2);
    switch (RandomSound)
    {
    case 0:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_fireBallImpact-001_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    case 1:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_fireBallImpact-002_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;
    case 2:
        m_pGameInstance->PlaySound_Z(_T("sfx_multi_fireBallImpact-003_soundWave.ogg"), SOUND_MONSTER, 0.4f);
        break;

    }

}

HRESULT CCircle::Add_Components()
{
    /*Prototype_Component_Texture_Circle*/
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Circle"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* For.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Circle"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;



    CBounding_Sphere::SPHERE_DESC spheredesc = {};
    spheredesc.fRadius = 0.5f;
    spheredesc.vCenter = _float3(0.f, 0.f, 0.f);;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &spheredesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CCircle::Bind_ShaderResources()
{

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fRatio", &m_fRatio, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    return S_OK;
}

CCircle* CCircle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CCircle* pInstance = new CCircle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : Circle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCircle::Clone(void* pArg)
{
    CCircle* pInstance = new CCircle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : Circle");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CCircle::Free()
{
    __super::Free();
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pColliderCom);

}
