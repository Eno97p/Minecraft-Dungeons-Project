#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize_Prototype(istream& os);
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Save_Texture(ostream& os);
public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

private:
	_uint													m_iNumTextures = { 0};
	vector<ID3D11ShaderResourceView*>						m_Textures;
	_tchar			m_szTextureFilePath[MAX_PATH] = TEXT("");
public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& strTextureFilePath, _uint iNumTextures);
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, istream& os);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END