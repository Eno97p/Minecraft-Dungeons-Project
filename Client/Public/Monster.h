#pragma once

#include "Client_Defines.h"
#include "GameObject.h"



BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CNavigation;
class CBehaviorTree;
class CTexture;
END

BEGIN(Client)

class CMonster : public CGameObject
{
public:
	enum MONSTER_ID {
		CREEPER, ZOMBIE, SKELETON, SKELETON_HORSE, FORKLIFT, FIONA, ENDERMAN,
		NPC, GOLEM, REDSTONEBOSS, GEOMANCER, VINDICATOR, ARCHILLAHER, HEARTOFENDER, HUSK, ITEM_BOX, DOOR, CHAIR,
		ID_END
	};
public:
	typedef struct MONSTER_DEC
	{
		_float Scale;
		_float3 Rotation;
		_float RotationAngle;
		_float4 Pos;
		MONSTER_ID ID;
	};


	typedef struct MonsterStatus
	{
		_float		fHp;
		_float		fMaxHp;
		_float		fDamage;
		_float		m_fRotationpersec;
		_float		m_fSpeed;
	};


public:
	void Set_Pick(_bool picked) { m_bIsPicked = picked; }
	HRESULT Set_Update(MONSTER_DEC* _dec);
	void Set_Pos(_vector _pos);
	MONSTER_DEC* Get_Dec() { return &m_MonsterDec; }
	_uint Get_NumAnim();
	void Set_AnimIndex();
	_int* Get_AnimIndex() { return &m_Animindex; }
	_bool InterSectFromWeapon(CCollider* pDstCollidercom);
	_bool InterSect(CCollider* pDstCollidercom);

	void MakeHitParticle(_uint iIndex);

protected:
	void CollisionEachOther(_float fTimeDelta);
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT SetStatus() = 0;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Add_Nodes() = 0;
	void LookAtPlayer();
	_vector Calculate_Player_Dir();
	HRESULT AddNavigation();
public:
	virtual HRESULT Render_LightDepth() override;

public:
	void Set_Damage(_float fDamage, _uint ParticleNum);
	_vector PlayerPos();



protected:		//공용 함수
	_bool IsPlayerApproach(_float fDistance);
	void MakeCircle(_float fRadius, _float fLifeTime,_uint ParticleNumber, CGameObject* pTarget = nullptr);
	void RandomCircle(_float fRadius, _float fLifeTime, _uint ParticleNumber, _float fDistance);


protected:		//공용 노드 함수
	virtual NodeStates Patroll(_float fTimeDelta) = 0;
	void TurnToPlayer(_float fTimeDelta);
	NodeStates Detect(_float fTimeDelta);

	virtual void HurtSound();




protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	_bool							m_bIsPicked = false;
	MONSTER_DEC						m_MonsterDec = {};
	_int							m_Animindex = 0;
	CCollider*						m_pColliderCom = { nullptr };
	CCollider*						m_pPlayerCollider = { nullptr };
	CNavigation*					m_pNavigationCom = { nullptr };
	CTransform*						m_pPlayerTransform = { nullptr };
	CBehaviorTree*					m_ComBehavior = { nullptr };
	_bool							m_IsHit = false;
	_float							m_HitReset = 0.2f;
	CGameObject*					m_pPlayer = nullptr;
	MonsterStatus*					m_pStatus = nullptr;

	_float							g_Threshold = 0.0f;
	_float							m_HitTimeRatio = 0.f;

	class CParticleManager*				m_pParticleManager = nullptr;
protected:
	_float		m_fFrameChange = 0.f;
	_float		m_fAcctime = 0.f;
	_bool		m_bIsDetected = false;
	_bool		m_bIdle = false;
	_float		m_Animspeed = 1.f;

	_float		m_DetectDist = 10.f;

	CGameObject* m_SmallHpBar = nullptr;
	_bool m_firsthit = false;

protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ShaderResources() = 0;

protected:
	HRESULT Create_Item(_uint itemIndex);
	HRESULT RandomDrop();
	HRESULT NamedDrop();
	HRESULT Create_Emerald(_uint iNumEmerald);

	void SwordHitSound();
	void AxeHitSound();
	void ClayMoreHitSound();
	void KatanaHitSound();
	void SpearHitSound();
public:
	virtual void Free() override;
};

END