#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CNavigation;
END

BEGIN(Client)
class CItem :	public CGameObject
{

public:
	typedef struct ItemDesc : public GAMEOBJECT_DESC
	{
		_int	RandomValue;
		_float fRadius;
		_float4 vPos;
		_float fData;
		_float3 vDir;
		const wchar_t* Item_Name;
		const wchar_t* TextureTag;
	};

private:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& rhs);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	void Set_Info();
	void CreateEpicBeam();
public:
	ItemDesc Get_ItemDesc() { return Description; }
private:
	CShader*		m_pShaderCom = { nullptr };
	CTexture*		m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CNavigation*	 m_pNavigationCom = { nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	_bool IsPlayerApproach(_float fDist);
	_vector Get_PlayerPos();
	void ItemParticle();

	_bool* Isdead = nullptr;
private:
	class CPlayer* pPlayer;
	ItemDesc Description = {};
public:
	static CItem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END