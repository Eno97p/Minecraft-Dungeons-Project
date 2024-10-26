#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CWeapon abstract : public CPartObject
{
public:
	typedef struct WEAPON_DESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;
	};

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;
	
public:
	virtual HRESULT Render_LightDepth() override;
	_vector Get_Collider_Center();
protected:
	virtual HRESULT Initialize(void* pArg) override;
protected:
	CCollider*						 m_pColliderCom = { nullptr };
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	const _float4x4*				m_pSocketMatrix = { nullptr };

public:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ShaderResources() = 0;
	void Bind_ProjResources(_float4x4* m_ViewMatrix, _float4x4* m_ProjMatrix);
	void RenderUI();
	void Socket_Matrix_Bind();
public:
	virtual void Free() override;
};

END