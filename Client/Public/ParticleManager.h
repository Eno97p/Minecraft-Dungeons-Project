#pragma once

#include "Client_Defines.h"
#include "VIBuffer_Instance.h"
#include "ParticleMesh.h"
BEGIN(Engine)
class CGameObject;
END


BEGIN(Client)
class CParticleManager : public CBase
{
	DECLARE_SINGLETON(CParticleManager)
public:
	CParticleManager();
	virtual ~CParticleManager() = default;
public:
	HRESULT Initialize();
	HRESULT Create_Particle(_uint iIndex, _vector vStart, _vector vDir = XMVectorZero());
	HRESULT Create_Particle(_uint iIndex, _vector vStart, CGameObject* pTarget = nullptr);
	HRESULT Create_Particle(_uint iIndex, _vector vStart, _vector vAxis, _float fRadian);
private:
	HRESULT Add_Desc(const CParticleMesh::PARTICLE_DESC& desc);

private:
	vector<CParticleMesh::PARTICLE_DESC*> vecDesc;

public:
	virtual void Free() override;

};

END