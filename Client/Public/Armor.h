#pragma once

#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CArmor abstract : public CPartObject
{
public:
	typedef struct ARMORDESC : public CPartObject::PARTOBJ_DESC
	{
		const _float4x4*			pCombinedTransformationMatrix;
	};

protected:
	CArmor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CArmor(const CArmor& rhs);
	virtual ~CArmor() = default;

protected:
	virtual HRESULT Initialize(void* pArg) override;
protected:
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