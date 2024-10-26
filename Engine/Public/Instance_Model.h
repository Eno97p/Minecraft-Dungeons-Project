#pragma once

#include "Component.h"
#include "VIBuffer_Instance.h"

BEGIN(Engine)

class ENGINE_DLL CInstance_Model final : public CComponent
{

private:
	CInstance_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	CInstance_Model(const CInstance_Model& rhs);
	virtual ~CInstance_Model() = default;

public:
	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);

public:
	virtual HRESULT	Initialize_Prototype(const  _char* pModelFilePath, _fmatrix PreTransformMatrix, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);
	HRESULT Bind_Material(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
private:
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	_float4x4					m_PreTransformMatrix;
	
private:
	_uint						m_iNumMeshes = { 0 };	
	vector<class CInstance_Mesh*>		m_Meshes;

private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;
	string						m_fullpath;


private:
	_bool						isFile = false;

	
private:
	HRESULT Ready_Meshes(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	
public:
	HRESULT Save_Model();
	HRESULT Load_Model(_fmatrix PreTransformMatrix, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	static CInstance_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, _fmatrix PreTransformMatrix, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END