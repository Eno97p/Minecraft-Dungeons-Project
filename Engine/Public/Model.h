#pragma once

#include "Component.h"
#include "Animation.h"
#include "VIBuffer_Instance.h"
BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MODELTYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_END };

	typedef struct tagAnimationDesc
	{
		tagAnimationDesc(_uint iAnimIndex, _bool isLoop)
			: iAnimIndex{ iAnimIndex }, isLoop{ isLoop } {}
		_uint		iAnimIndex = { 0 };
		_bool		isLoop = { false };
	}ANIMATION_DESC;
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	void Spread(_float fTimeDelta);
	void Drop(_float fTimeDelta);
	void GrowOut(_float fTimeDelta);
	void Spread_Size_Up(_float fTimeDelta);
	void Spread_Non_Rotation(_float fTimeDelta);
	void CreateSwirlEffect(_float fTimeDelta);
	void Spread_Speed_Down(_float fTimeDelta);
	void SlashEffect(_float fTimeDelta);
	void Spread_Speed_Down_SizeUp(_float fTimeDelta);
	void Gather(_float fTimeDelta);
	void Extinction(_float fTimeDelta);
	void GrowOutY(_float fTimeDelta);
	_bool Check_Instance_Dead();

	HRESULT Ready_Instance(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}
	_uint Get_NumAnim() const {
		return m_iNumAnimations;
	}

	_bool Get_AnimFinished() const {
		return m_Animations[m_AnimDesc.iAnimIndex]->Get_Finished();
	}

	_double Get_Current_Ratio();
	_bool Get_Ratio_Betwin(_float min, _float max);

	void StopAnimation(_bool _stop) { m_bAnimStop = _stop; }
	

	const _float4x4* Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const;

public:
	virtual HRESULT	Initialize_Prototype(MODELTYPE eModelType, const  _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT Render(_uint iMeshIndex);
	HRESULT Render_Instance(_uint iMeshIndex);

	HRESULT Bind_Material(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_Material_Instance(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType);
	HRESULT Bind_BoneMatrices(class CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex);
	
	void Play_Animation(_float fTimeDelta);
	void Set_AnimationIndex(const ANIMATION_DESC& AnimDesc); /*{
		if (m_AnimDesc.iAnimIndex == AnimDesc.iAnimIndex)
			return;
		m_AnimDesc = AnimDesc;
		m_Animations[m_AnimDesc.iAnimIndex]->Reset();
	}*/
	vector<string> Get_AnimNameVec();
	vector<string> Get_BoneNameVec();
	HRESULT Save_AnimName();
	HRESULT Save_BoneName();

	_bool Picking(class CTransform* pTransform, _float3* pOut);
private:
	const aiScene*				m_pAIScene = { nullptr };
	Assimp::Importer			m_Importer;
	_float4x4					m_PreTransformMatrix;
	MODELTYPE					m_eModelType = { TYPE_END };
	_float4x4					m_MeshBoneMatrices[512];
	
private:
	_uint						m_iNumMeshes = { 0 };	
	vector<class CMesh*>		m_Meshes;
	vector<class CInstance_Mesh*>	m_InstanseMesh;

private:
	_uint						m_iNumMaterials = { 0 };
	vector<MESH_MATERIAL>		m_Materials;


private:
	vector<class CBone*>		m_Bones;

private:
	_uint						m_iNumAnimations = { 0 };
	ANIMATION_DESC				m_AnimDesc{0, true};
	vector<class CAnimation*>	m_Animations;


	string						m_fullpath;
	_bool						m_bAnimStop = false;

	CAnimation* m_pNextAnimation = nullptr;
	CAnimation* m_pCurrentAnimation = nullptr;
private:
	_bool						isFile = false;


private:
	HRESULT Ready_Meshes();
	
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();
	


public:
	HRESULT Save_Model();
	HRESULT Load_Model(_fmatrix PreTransformMatrix);
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END