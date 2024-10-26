#pragma once

#include "Tool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(TOOL)

class CMonster : public CGameObject
{
public:
	enum MONSTER_ID { CREEPER, ZOMBIE,SKELETON, SKELETON_HORSE, FORKLIFT,FIONA,ENDERMAN,
		NPC,GOLEM,REDSTONEBOSS,GEOMANCER,VINDICATOR,ARCHILLAHER,HEARTOFENDER, HUSK, ITEM_BOX, DOOR,
		CHAIR,
		ID_END };
public:
	typedef struct MONSTER_DEC
	{
		_float Scale;
		_float3 Rotation;
		_float RotationAngle;
		_float4 Pos;
		MONSTER_ID ID;
	};
public:
	void Set_Pick(_bool picked) { m_bIsPicked = picked; }
	HRESULT Set_Update(MONSTER_DEC* _dec);
	void Set_Pos(_vector _pos);
	MONSTER_DEC* Get_Dec() { return &m_MonsterDec; }
	_uint Get_NumAnim();
	void Set_AnimIndex();
	_int* Get_AnimIndex() { return &m_Animindex; }

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
protected:
	CShader*						m_pShaderCom = { nullptr };
	CModel*							m_pModelCom = { nullptr };
	_bool							m_bIsPicked = false;
	MONSTER_DEC						m_MonsterDec = {};
	_int							m_Animindex = 0;

protected:
	virtual HRESULT Add_Components() = 0;
	virtual HRESULT Bind_ShaderResources() = 0;
public:
	virtual void Free() override;
};

END