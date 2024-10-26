#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUI :  public CGameObject
{
public:
	typedef struct UIDESC : public GAMEOBJECT_DESC
	{
		_int a;
	};

protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
public:
	RECT* Get_CollisionRect() { return &m_CollisionRect; }
	_bool CollisionRect(RECT* SrcRect);
	_float4 Get_Position_Value() {
		return _float4(m_fX, m_fY, m_fSizeX, m_fSizeY);
	}
	virtual void Resize(_float4 PositionValue);
	
protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;


	RECT m_CollisionRect = {};
public:
	virtual void Free() override;
};

END