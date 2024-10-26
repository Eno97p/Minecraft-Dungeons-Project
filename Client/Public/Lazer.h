#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CLazer :	 public CGameObject
{
public:
	enum LAZERDIRECTION { LOOK, RIGHT_LOOK, RIGHT_LOOKMINUS, LEFT_LOOK, LEFT_LOOKMINUS, LAZERDIRCTION_END};
public:
	typedef struct LAZERDESC : public GAMEOBJECT_DESC
	{
		const XMFLOAT4X4* ParentMatrix;
		_float vZSize;
		_float	fStartRotation;
		_float LifeTime;
		LAZERDIRECTION eDirType = LOOK;
	};

private:
	CLazer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLazer(const CLazer& rhs);
	virtual ~CLazer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
private:
	void Direction_Calculator(_float fTimeDelta);


private:
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pPlayerCollider = { nullptr };
	class CPlayer* m_pPlayer = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_float  m_MaxSize = 0.3f;
	_float	m_XSize = 0.f;
	_float	m_ZSize = 0.f;
	_float	m_OriginZsize = 0.f;
	_float m_LifeTime = 0.f;
	_bool	LazerDead = false;
	const _float4x4* m_ParentMatrix;

	_vector PreLook;
	LAZERDIRECTION m_eDirType;
public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLazer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;



};

END