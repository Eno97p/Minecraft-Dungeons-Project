#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"
#include "VIBuffer_Instance.h"
BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CParticleMesh : public CBlendObject
{
public:
	typedef struct PARTICLE_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		XMFLOAT4							    vStartPos;
		CVIBuffer_Instance::INSTANCE_DESC      InstanceDesc;
		XMFLOAT3						        vStartColor;
		XMFLOAT3						        vEndColor;
		EFFECTTYPE						    eType;
		EFFECTMODELTYPE						eModelType;
		_bool								IsBlur = false;
	};
private:
	CParticleMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleMesh(const CParticleMesh& rhs);
	virtual ~CParticleMesh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void Set_Target(CGameObject* pTarget)
	{
		m_pTarget = pTarget;
	}
	void Set_Rotaition(_float Radian, _vector Axis);

private:
	CShader* m_pShaderCom = { nullptr };
	CModel*	 m_InstModelCom = { nullptr };

private:
	HRESULT Add_Components(const wstring& strModelPrototype);
	HRESULT Bind_ShaderResources();

	EFFECTTYPE		m_eType;
	_float3		vStartColor;
	_float3		vEndColor;
	_int		m_ShaderPass = 0;
	const wchar_t* m_ModelPrototypeTag = nullptr;

	CGameObject* m_pTarget = nullptr;
	_bool m_IsBlur = false;
public:
	static CParticleMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END