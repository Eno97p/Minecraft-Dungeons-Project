#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
	{
		_int iData;
	};

protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	/*  지정한 이름의 컴포넌트를 가져온다. */
	class CComponent* Get_Component(const wstring& strComponentTag);
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Tick(_float fTimeDelta);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_LightDepth() { return S_OK; }
	void Set_Unique_ID(const char* _ID) { m_pUniqueId = _ID; }
	const char* Get_Unique_ID() { return m_pUniqueId; }
	void Set_Dead() { IsDead = true; }
	_bool Get_Dead() { return IsDead; }
	_matrix Get_WorldMat() { return m_pTransformCom->Get_WorldMatrix(); }
	HRESULT Set_WorldMat(_float4x4 worldmat);
	vector<string> Get_AnimNameVec();
	_bool InterSected(CGameObject* Dst);




	virtual _bool Mesh_Picking(_float3* PickingPoint) { return true; }
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	bool checkCondition() {
		srand(time(nullptr));
		return rand() % 2 == 1;
	}
	

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };
	class CTransform*			m_pTransformCom = { nullptr };
	static const _tchar*		m_pTransformTag;
	const char*						m_pUniqueId = nullptr;

protected:
	map<const wstring, class CComponent*>		m_Components;

protected:
	HRESULT Add_Component(_uint iPrototypeLevelIndex, const wstring& strPrototypeTag, const wstring& strComponentTag, 
		CComponent** ppOut, void* pArg = nullptr);

private:
	_bool IsDead = false;

protected:
	_bool IsCloned = false;

public:	
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END