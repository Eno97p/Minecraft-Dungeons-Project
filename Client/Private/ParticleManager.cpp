#include "stdafx.h"
#include "ParticleManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CParticleManager)

CParticleManager::CParticleManager()
{
}

HRESULT CParticleManager::Initialize()
{











    string finalPath = "../Bin/BinaryFile/Effect/Effect.Bin";
    ifstream inFile(finalPath, std::ios::binary);
    if (!inFile.good())		//경로 안에 파일이 없으면
    {
        return E_FAIL;
    }
   

    _uint iSize = 0;
    inFile.read((char*)&iSize, sizeof(_uint));
    for (_uint i = 0; i < iSize; ++i)
    {
        CParticleMesh::PARTICLE_DESC desc{};
        inFile.read((char*)&desc.eType, sizeof(EFFECTTYPE));
        inFile.read((char*)&desc.InstanceDesc, sizeof(CVIBuffer_Instance::INSTANCE_DESC));
        inFile.read((char*)&desc.eModelType, sizeof(EFFECTMODELTYPE));
        inFile.read((char*)&desc.vStartColor, sizeof(_float3));
        inFile.read((char*)&desc.vEndColor, sizeof(_float3));
        inFile.read((char*)&desc.IsBlur, sizeof(_bool));
        desc.vStartPos = _float4(0.f, 0.f, 0.f, 1.f);
        Add_Desc(desc);
    }

    inFile.close();
    
    return S_OK;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, _vector vDir)
{
    XMStoreFloat4(&vecDesc[iIndex]->vStartPos , vStart);

    CGameObject* ParticleMesh = nullptr;

    ParticleMesh = GetGI->Clone_Object(TEXT("Prototype_GameObject_Particle_Mesh"), vecDesc[iIndex]);
    GetGI->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), ParticleMesh);

    return S_OK;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, CGameObject* pTarget)
{
    XMStoreFloat4(&vecDesc[iIndex]->vStartPos, vStart);

    CGameObject* ParticleMesh = nullptr;

    ParticleMesh = GetGI->Clone_Object(TEXT("Prototype_GameObject_Particle_Mesh"), vecDesc[iIndex]);
    static_cast<CParticleMesh*>(ParticleMesh)->Set_Target(pTarget);
    GetGI->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), ParticleMesh);

    return S_OK;
}

HRESULT CParticleManager::Create_Particle(_uint iIndex, _vector vStart, _vector vAxis, _float fRadian)
{
    XMStoreFloat4(&vecDesc[iIndex]->vStartPos, vStart);

    CGameObject* ParticleMesh = nullptr;

    ParticleMesh = GetGI->Clone_Object(TEXT("Prototype_GameObject_Particle_Mesh"), vecDesc[iIndex]);
    static_cast<CParticleMesh*>(ParticleMesh)->Set_Rotaition(fRadian, vAxis);
    GetGI->CreateObject(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), ParticleMesh);

    return S_OK;
}

HRESULT CParticleManager::Add_Desc(const CParticleMesh::PARTICLE_DESC& desc)
{
    CParticleMesh::PARTICLE_DESC* newDesc = new CParticleMesh::PARTICLE_DESC(desc);
    vecDesc.emplace_back(newDesc);
    return S_OK;
}

void CParticleManager::Free()
{
    for (auto& iter : vecDesc)
    {
        Safe_Delete(iter);
    }
}
