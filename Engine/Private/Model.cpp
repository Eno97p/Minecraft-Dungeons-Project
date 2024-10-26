#include "..\Public\Model.h"

#include "Bone.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Instance_Mesh.h"
CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _char* pModelFilePath)
	: CComponent { pDevice, pContext }
	, m_fullpath{ pModelFilePath }
{

}

CModel::CModel(const CModel & rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes { rhs.m_Meshes } 
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials { rhs.m_Materials }
	, m_eModelType{ rhs.m_eModelType }
	, m_PreTransformMatrix{ rhs.m_PreTransformMatrix }
	, m_iNumAnimations{rhs.m_iNumAnimations }
	, m_fullpath{rhs.m_fullpath }
	, isFile{rhs.isFile }
{
	for (auto& pPrototypeAnimation : rhs.m_Animations)
		m_Animations.emplace_back(pPrototypeAnimation->Clone());

	for (auto& pPrototypeBone : rhs.m_Bones)
		m_Bones.emplace_back(pPrototypeBone->Clone());		

	for (auto& MaterialDesc: m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(MaterialDesc.MaterialTextures[i]);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}


_double CModel::Get_Current_Ratio()
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_Current_Ratio();
}

_bool CModel::Get_Ratio_Betwin(_float min, _float max)
{
	return (Get_Current_Ratio() > min && Get_Current_Ratio() < max);
}

const _float4x4* CModel::Get_BoneCombinedTransformationMatrix(const _char* pBoneName) const
{
	auto	iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)->_bool
		{
			return pBone->Compare_Name(pBoneName);
		});

	if (iter == m_Bones.end())
		return nullptr;

	return (*iter)->Get_CombinedTransformationMatrix();
}

HRESULT CModel::Initialize_Prototype(MODELTYPE eModelType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{	
	if (FAILED(Load_Model(PreTransformMatrix)))
		return E_FAIL;

	if (isFile == true) //isFile�� true�� �ؿ��� ���� ����
		return S_OK;
	/*aiProcessPreset_TargetRealtime_Fast*/
	_uint		iFlag;
	if (TYPE_NONANIM == eModelType)
	{
		/*iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals
			| aiProcess_CalcTangentSpace;*/
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;
	}
	else
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace |
		aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | 0;

	/* �޽õ��� �����ѻ��� ��ġ�� �̸� �� ��ġ��Ų��. */
	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_eModelType = eModelType;	

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	if (FAILED(Ready_Bones(m_pAIScene->mRootNode, -1)))
		return E_FAIL;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Ready_Animations()))
		return E_FAIL;		

	if (FAILED(Save_Model()))
		return E_FAIL;


	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{


	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{	
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();
	
	return S_OK;
}

HRESULT CModel::Render_Instance(_uint iMeshIndex)
{
	m_InstanseMesh[iMeshIndex]->Bind_Buffers();
	m_InstanseMesh[iMeshIndex]->Render();
	return S_OK;
}

HRESULT CModel::Bind_Material(CShader * pShaderCom, const _char * pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].MaterialTextures[eMaterialType]->Bind_ShaderResource(pShaderCom, pConstantName, 0);
}

HRESULT CModel::Bind_Material_Instance(CShader* pShaderCom, const _char* pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	return m_Materials[m_InstanseMesh[iMeshIndex]->Get_MaterialIndex()].MaterialTextures[eMaterialType]->Bind_ShaderResource(pShaderCom, pConstantName, 0);
}

HRESULT CModel::Bind_BoneMatrices(CShader * pShaderCom, const _char * pConstantName, _uint iMeshIndex)
{
	ZeroMemory(m_MeshBoneMatrices, sizeof(_float4x4) * 512);

	m_Meshes[iMeshIndex]->Fill_Matrices(m_Bones, m_MeshBoneMatrices);

	return pShaderCom->Bind_Matrices(pConstantName, m_MeshBoneMatrices, 512);		
}

void CModel::Play_Animation(_float fTimeDelta)
{
	/* Ư�� �ִϸ��̼��� ����Ѵ�. == Ư�� �ִϸ��̼ǿ��� ����ϴ� ������ TransformationMatrix�� �������ش�. */
	/* ���� �ִϸ��̼��� ���¿� �µ��� ������ �������(TransformationMatrix)�� ����� �������ش�. */
	/* m_Animations[m_iCurrentAnimIndex] : �� �ִϸ��̼ǿ��� ��뤷�ϴ� ������ ���������� */
	if (!m_bAnimStop)
	{
		if (!m_pNextAnimation) m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop);
		else
		{
			if (!m_pCurrentAnimation->Lerp_NextAnimation(0.2f, m_pNextAnimation, m_Bones))
			{
				m_pNextAnimation = nullptr;
			} // 0.2�� ���� ���� ���� ����
		}

		/*m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_Bones, m_AnimDesc.isLoop);*/
	}
	/* ��ü���� ��ȸ�ϸ鼭 ��� ���� CombinedTransformationMatrix�� �����Ѵ�. */
	for (auto& pBone : m_Bones)
		pBone->Update_CombinedTransformationMatrix(m_Bones, XMLoadFloat4x4(&m_PreTransformMatrix));
}
void CModel::Set_AnimationIndex(const ANIMATION_DESC& AnimDesc)
{
	if (m_AnimDesc.iAnimIndex == AnimDesc.iAnimIndex)
	{
		if (m_bAnimStop)
		{
			m_AnimDesc = AnimDesc;
		}
		return;
	}
	m_pNextAnimation = m_Animations[AnimDesc.iAnimIndex];
	m_pCurrentAnimation = m_Animations[m_AnimDesc.iAnimIndex];

	m_AnimDesc = AnimDesc;
	m_Animations[m_AnimDesc.iAnimIndex]->Reset(m_Bones);

}



vector<string> CModel::Get_AnimNameVec()
{
	vector<string> _Names;
	for (auto& iter : m_Animations)
	{
		_Names.push_back(iter->Get_Name());
	}
	return _Names;
}

vector<string> CModel::Get_BoneNameVec()
{
	vector<string> _Names;
	for (auto& iter : m_Bones)
	{
		_Names.push_back(iter->Get_Name());
	}
	return _Names;

}

HRESULT CModel::Save_AnimName()
{
	vector<string> _Names = Get_AnimNameVec();
	string binaryFile = "../Bin/BinaryFile/Text/";
	// ���� �̸��� ���Ե� ���ڿ�

	// ���� �̸����� ���丮 ��θ� ������ �κ� ����
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// Ȯ���ڸ� bin���� ����
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".txt";
	}
	// �������� ��� ����
	binaryFile += filenamePart;

	ofstream file(binaryFile);
	if (file.is_open())
	{
		for (size_t i = 0; i < _Names.size(); ++i) {
			file <<"("<< i << ")"<< " " << _Names[i] << "\n"; // ��ȣ�� ���ڿ��� �ٹٲ����� �����Ͽ� ����
		}
		file.close();
		return S_OK;
	}
	else
	{
		MSG_BOX("Failed to save animtext");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CModel::Save_BoneName()
{
	vector<string> _Names = Get_BoneNameVec();
	string binaryFile = "../Bin/BinaryFile/Bones/";
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// Ȯ���ڸ� bin���� ����
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".txt";
	}
	// �������� ��� ����
	binaryFile += filenamePart;

	ofstream file(binaryFile);
	if (file.is_open())
	{
		for (size_t i = 0; i < _Names.size(); ++i) {
			file << "(" << i << ")" << " " << _Names[i] << "\n"; // ��ȣ�� ���ڿ��� �ٹٲ����� �����Ͽ� ����
		}
		file.close();
		return S_OK;
	}
	else
	{
		MSG_BOX("Failed to save Bonetext");
		return E_FAIL;
	}

	return S_OK;
}

_bool CModel::Picking(CTransform* pTransform, _float3* pOut)
{
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		if (m_Meshes[i]->Picking(pTransform, pOut))
			return true;
	}

	return false;
}

HRESULT CModel::Ready_Meshes()
{
	/* �ϳ��� ���� �������� �޽÷� �����Ǿ��ִ�. */
	/* �޽� : �������� ����. */
	/* ������ : ���� ����. */

	/* -> �޽ø� ����� == �������۸� �����. + �ε��� */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix), m_Bones);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

void CModel::Spread(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Spread(fTimeDelta);
	
}

void CModel::Drop(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Drop(fTimeDelta);
}

void CModel::GrowOut(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->GrowOut(fTimeDelta);
}

void CModel::Spread_Size_Up(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Spread_Size_Up(fTimeDelta);
}

void CModel::Spread_Non_Rotation(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Spread_Non_Rotation(fTimeDelta);
}

void CModel::CreateSwirlEffect(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->CreateSwirlEffect(fTimeDelta);
}

void CModel::Spread_Speed_Down(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Spread_Speed_Down(fTimeDelta);
}

void CModel::SlashEffect(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->SlashEffect(fTimeDelta);
}

void CModel::Spread_Speed_Down_SizeUp(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Spread_Speed_Down_SizeUp(fTimeDelta);
}

void CModel::Gather(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Gather(fTimeDelta);
}

void CModel::Extinction(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->Extinction(fTimeDelta);
}

void CModel::GrowOutY(_float fTimeDelta)
{
	for (auto& iter : m_InstanseMesh)
		iter->GrowOutY(fTimeDelta);
}

_bool CModel::Check_Instance_Dead()
{
	for (auto& iter : m_InstanseMesh)
	{
		if (!iter->Check_Instance_Dead())
			return false;
	}
	return true;
}

HRESULT CModel::Ready_Instance(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{
	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CInstance_Mesh* pInstance = CInstance_Mesh::Create(m_pDevice, m_pContext, m_Meshes[i], InstanceDesc);
		if (nullptr == pInstance)
			return E_FAIL;

		m_InstanseMesh.emplace_back(pInstance);
	}
	return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* pModelFilePath)
{
	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		aiMaterial*		pAIMaterial = m_pAIScene->mMaterials[i];

		MESH_MATERIAL		MeshMaterial{};

		for (size_t j = aiTextureType_DIFFUSE; j < AI_TEXTURE_TYPE_MAX; j++)
		{
			aiString		strTextureFilePath;
			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strTextureFilePath)))
				continue;

			_char		szFileName[MAX_PATH] = "";
			_char		szExt[MAX_PATH] = "";

			_splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);			

			_char		szDrive[MAX_PATH] = "";
			_char		szDirectory[MAX_PATH] = "";

			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);

			_char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szDirectory);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szRealFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, strlen(szFullPath), szRealFullPath, MAX_PATH);
			
			MeshMaterial.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, szRealFullPath, 1);
			if (nullptr == MeshMaterial.MaterialTextures[j])
				return E_FAIL;
		}

		m_Materials.emplace_back(MeshMaterial);
	}

	return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* pAINode, _int iParentIndex)
{
	CBone*		pBone = CBone::Create(pAINode, iParentIndex);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.emplace_back(pBone);

	_uint		iParent = m_Bones.size() - 1;

	for (_uint i = 0; i < pAINode->mNumChildren; ++i)
	{
		Ready_Bones(pAINode->mChildren[i], iParent);
	}

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{	
	/* � �ִϸ��̼��� ������� ��, �� �ִϸ��̼ǿ��� ����ϰ� �ִ� ������ ������ ��ȯ ����(���)���� �ε��Ѵ�.  */
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation*		pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], m_Bones);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.emplace_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Save_Model()
{
	string binaryFile = "../Bin/BinaryFile/";
	// ���� �̸��� ���Ե� ���ڿ�
	
	// ���� �̸����� ���丮 ��θ� ������ �κ� ����
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// Ȯ���ڸ� bin���� ����
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".bin";
	}

	// �������� ��� ����
	binaryFile += filenamePart;
	
	ofstream file(binaryFile ,ios::out | std::ios::binary);
	//binaryFile = ��� + �̸�, ���⿡ �Ȱ��� ������ ������ �����, ������ ���� �����
	if (file.is_open()) {
		
		//���� ���� �κ�
		file.write(reinterpret_cast<char*>(&m_PreTransformMatrix), sizeof(_float4x4));
		file.write(reinterpret_cast<char*>(&m_AnimDesc), sizeof(ANIMATION_DESC));


		_uint bonesize = m_Bones.size(); //60
		file.write((char*)&bonesize, sizeof(_uint));
		for (auto& bone : m_Bones)
			bone->Save_Bone(file);


		
		file.write((char*)&m_iNumMeshes, sizeof(_uint));
		for (auto& Mesh : m_Meshes)
			Mesh->Save_Mesh(file);
		
		file.write((char*)&m_iNumMaterials, sizeof(_uint)); //4
		for (auto& SaveMaterial : m_Materials)
		{
			for (size_t i = 1; i < AI_TEXTURE_TYPE_MAX; i++)
			{
				if (nullptr != SaveMaterial.MaterialTextures[i])
				{
					_uint check = 1;
					file.write(reinterpret_cast<char*>(&check), sizeof(_uint));
					SaveMaterial.MaterialTextures[i]->Save_Texture(file);
				}
				else
				{
					_uint check = 0;
					file.write(reinterpret_cast<char*>(&check), sizeof(_uint));
				}
			}
		}

		file.write((char*)&m_iNumAnimations, sizeof(_uint)); //25

		_uint AnimSize = m_Animations.size();
		file.write((char*)&AnimSize, sizeof(_uint));
		for (auto& Anim : m_Animations)
			Anim->Save_Anim(file);


		file.close();
	}
	else
	{
		MSG_BOX("Failed To OpenFile");
		file.close();
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CModel::Load_Model(_fmatrix PreTransformMatrix)
{
	string binaryFile = "../Bin/BinaryFile/";
	// ���� �̸��� ���Ե� ���ڿ�

	// ���� �̸����� ���丮 ��θ� ������ �κ� ����
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// Ȯ���ڸ� bin���� ����
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".bin";
	}

	// �������� ��� ����
	binaryFile += filenamePart;

	ifstream inFile(binaryFile, std::ios::binary);
	if (!inFile.good())		//��� �ȿ� ������ ������
	{
		isFile = false;		//isFile �� false�ϰ� ����
		return S_OK;
	}
	else
		isFile = true;		//isFile�� true�ϰ� �������

	if (!inFile.is_open()) {
		// ���� ���� ���� ����
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		inFile.read(reinterpret_cast<char*>(&m_PreTransformMatrix), sizeof(_float4x4));
		inFile.read(reinterpret_cast<char*>(&m_AnimDesc), sizeof(ANIMATION_DESC));

		_uint bonenum = 0;
		inFile.read((char*)&bonenum, sizeof(_uint));
		for (size_t i = 0; i < bonenum; ++i)
		{
			CBone* bone = CBone::LoadCreate(inFile);
			m_Bones.push_back(bone);
		}

		
		inFile.read((char*)&m_iNumMeshes, sizeof(_uint));
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			CMesh* bone = CMesh::LoadCreate(m_pDevice,m_pContext,inFile);
			m_Meshes.push_back(bone);
		}


		inFile.read((char*)&m_iNumMaterials, sizeof(_uint));
		for (size_t i = 0; i < m_iNumMaterials; i++)
		{
			MESH_MATERIAL pMt{};
			for (size_t j = 1; j < AI_TEXTURE_TYPE_MAX; j++)
			{
				_uint check;
				inFile.read(reinterpret_cast<char*>(&check), sizeof(_uint));
				if (1 == check)
				{
					pMt.MaterialTextures[j] = CTexture::Create(m_pDevice, m_pContext, inFile);
					if (nullptr == pMt.MaterialTextures[j])
						return E_FAIL;
				}
			}
			m_Materials.emplace_back(pMt);
		}

		inFile.read((char*)&m_iNumAnimations, sizeof(_uint));
		_uint AnimSize = 0;
		inFile.read((char*)&AnimSize, sizeof(_uint));
		for (size_t i = 0; i < AnimSize; ++i)
		{
			CAnimation* Anim = CAnimation::LoadCreate(inFile);
			m_Animations.emplace_back(Anim);
		}
		Save_AnimName();
		Save_BoneName();
	}
	inFile.close();

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, MODELTYPE eModelType, const _char * pModelFilePath, _fmatrix PreTransformMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext, pModelFilePath);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pModelFilePath, PreTransformMatrix)))
	{
		MSG_BOX("Failed To Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CModel::Free()
{
	__super::Free();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);

	m_Bones.clear();

	for (auto& MaterialDesc : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(MaterialDesc.MaterialTextures[i]);
	}

	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& pMesh : m_InstanseMesh)
		Safe_Release(pMesh);

	m_InstanseMesh.clear();


	m_Importer.FreeScene();
}
