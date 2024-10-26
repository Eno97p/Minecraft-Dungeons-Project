#include "Instance_Model.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Instance_Mesh.h"


CInstance_Model::CInstance_Model(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _char* pModelFilePath)
	: CComponent { pDevice, pContext }
	, m_fullpath{ pModelFilePath }
{

}

CInstance_Model::CInstance_Model(const CInstance_Model & rhs)
	: CComponent{ rhs }
	, m_iNumMeshes{ rhs.m_iNumMeshes }
	, m_Meshes { rhs.m_Meshes } 
	, m_iNumMaterials{ rhs.m_iNumMaterials }
	, m_Materials { rhs.m_Materials }
	, m_PreTransformMatrix{ rhs.m_PreTransformMatrix }
	, m_fullpath{rhs.m_fullpath }
	, isFile{rhs.isFile }
{
	
	for (auto& MaterialDesc: m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(MaterialDesc.MaterialTextures[i]);
	}

	for (auto& pMesh : m_Meshes)
		Safe_AddRef(pMesh);
}



void CInstance_Model::Spread(_float fTimeDelta)
{
	for (auto& pMesh : m_Meshes)
		pMesh->Spread(fTimeDelta);

}

void CInstance_Model::Drop(_float fTimeDelta)
{
	for (auto& pMesh : m_Meshes)
		pMesh->Drop(fTimeDelta);
}


HRESULT CInstance_Model::Initialize_Prototype(const _char * pModelFilePath, _fmatrix PreTransformMatrix , const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{	
	if (FAILED(Load_Model(PreTransformMatrix , InstanceDesc)))
		return E_FAIL;

	if (isFile == true) //isFile이 true면 밑에꺼 수행 안함
		return S_OK;
	
	_uint		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;


	m_pAIScene = m_Importer.ReadFile(pModelFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;


	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);


	if (FAILED(Ready_Meshes(InstanceDesc)))
		return E_FAIL;

	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;	

	//if (FAILED(Save_Model()))
	//	return E_FAIL;


	return S_OK;
}

HRESULT CInstance_Model::Initialize(void* pArg)
{

	return S_OK;
}



HRESULT CInstance_Model::Render(_uint iMeshIndex)
{	
	m_Meshes[iMeshIndex]->Bind_Buffers();
	m_Meshes[iMeshIndex]->Render();
	
	return S_OK;
}

HRESULT CInstance_Model::Bind_Material(CShader * pShaderCom, const _char * pConstantName, _uint iMeshIndex, aiTextureType eMaterialType)
{
	return m_Materials[m_Meshes[iMeshIndex]->Get_MaterialIndex()].MaterialTextures[eMaterialType]->Bind_ShaderResource(pShaderCom, pConstantName, 0);
}



HRESULT CInstance_Model::Ready_Meshes(const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{
	/* 하나의 모델은 여러개의 메시로 구성되어있다. */
	/* 메시 : 폴리곤의 집합. */
	/* 폴리곤 : 정점 세개. */

	/* -> 메시를 만든다 == 정점버퍼를 만든다. + 인덱스 */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CInstance_Mesh*		pMesh = CInstance_Mesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i], XMLoadFloat4x4(&m_PreTransformMatrix), InstanceDesc);
		if (nullptr == pMesh)
			return E_FAIL;

		m_Meshes.emplace_back(pMesh);
	}

	return S_OK;
}

HRESULT CInstance_Model::Ready_Materials(const _char* pModelFilePath)
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


HRESULT CInstance_Model::Save_Model()
{
	string binaryFile = "../Bin/BinaryFile/InstanceModel/";
	// 파일 이름이 포함된 문자열
	
	// 파일 이름에서 디렉토리 경로를 제외한 부분 추출
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// 확장자를 bin으로 변경
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".bin";
	}

	// 최종적인 경로 조합
	binaryFile += filenamePart;
	
	ofstream file(binaryFile ,ios::out | std::ios::binary);
	//binaryFile = 경로 + 이름, 여기에 똑같은 파일이 있으면 덮어쓰고, 없으면 새로 만든다
	if (file.is_open()) {
		
		//파일 쓰는 부분
		
		
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

HRESULT CInstance_Model::Load_Model(_fmatrix PreTransformMatrix, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{
	string binaryFile = "../Bin/BinaryFile/InstanceModel/";
	// 파일 이름이 포함된 문자열

	// 파일 이름에서 디렉토리 경로를 제외한 부분 추출
	std::string filenamePart(m_fullpath);
	size_t lastSlashIndex = filenamePart.find_last_of('/');
	if (lastSlashIndex != std::string::npos) {
		filenamePart = filenamePart.substr(lastSlashIndex + 1);
	}

	// 확장자를 bin으로 변경
	size_t lastDotIndex = filenamePart.find_last_of('.');
	if (lastDotIndex != std::string::npos) {
		filenamePart = filenamePart.substr(0, lastDotIndex) + ".bin";
	}

	// 최종적인 경로 조합
	binaryFile += filenamePart;

	ifstream inFile(binaryFile, std::ios::binary);
	if (!inFile.good())		//경로 안에 파일이 없으면
	{
		isFile = false;		//isFile 을 false하고 리턴
		return S_OK;
	}
	else
		isFile = true;		//isFile을 true하고 계속진행

	if (!inFile.is_open()) {
		// 이진 파일 열기 실패
		MSG_BOX("Failed To Open File");
		return E_FAIL;
	}
	else
	{
		
		

		
		inFile.read((char*)&m_iNumMeshes, sizeof(_uint));
		for (size_t i = 0; i < m_iNumMeshes; ++i)
		{
			CInstance_Mesh* Mesh = CInstance_Mesh::LoadCreate(m_pDevice,m_pContext,inFile , InstanceDesc);
			m_Meshes.push_back(Mesh);
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

	}
	inFile.close();

	XMStoreFloat4x4(&m_PreTransformMatrix, PreTransformMatrix);

	return S_OK;
}

CInstance_Model * CInstance_Model::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext,  const _char * pModelFilePath, _fmatrix PreTransformMatrix, const CVIBuffer_Instance::INSTANCE_DESC& InstanceDesc)
{
	CInstance_Model*		pInstance = new CInstance_Model(pDevice, pContext, pModelFilePath);

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath, PreTransformMatrix, InstanceDesc)))
	{
		MSG_BOX("Failed To Created : CInstance_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CInstance_Model::Clone(void * pArg)
{
	CInstance_Model*		pInstance = new CInstance_Model(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CInstance_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CInstance_Model::Free()
{
	__super::Free();


	for (auto& MaterialDesc : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(MaterialDesc.MaterialTextures[i]);
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();
	m_Importer.FreeScene();
}
