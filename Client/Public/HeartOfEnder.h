#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CHeartOfEnder final : public CMonster
{
	enum HEARTOFENDERSTATES { STATE_IDLE, STATE_SUMMON_VISAGE ,STATE_INTRO, 
		STATE_SPLIT, STATE_FOURSIDE, STATE_WALK , STATE_APPEAR , STATE_TELEPORT , STATE_DEFEAT, STATE_END };
private:
	CHeartOfEnder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHeartOfEnder(const CHeartOfEnder& rhs);
	virtual ~CHeartOfEnder() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual HRESULT SetStatus() override;
public:
	virtual HRESULT Add_Components() override;
	HRESULT AddPartsObjects();
	virtual HRESULT Bind_ShaderResources() override;
private:
	virtual HRESULT Add_Nodes() override;
	virtual NodeStates Patroll(_float fTimeDelta) override;
	void SetAnimIndex(_float fTimeDelta);
	void Generate_RSS();
	void Generate_Fourside_Lazer();

private:
	NodeStates Defeat(_float fTimeDelta);


	NodeStates RandomPattern(_float fTimeDelta);
	NodeStates Summon(_float fTimeDelta);
	NodeStates FourSideIntro(_float fTimeDelta);
	NodeStates FourSide(_float fTimeDelta);
	NodeStates Split(_float fTimeDelta);

	NodeStates SpiderWalk(_float fTimeDelta);
	NodeStates IDLE(_float fTimeDelta);

	void DeafeatExplosion();

private:
	void Teleport();
	void Create_Core();
	void Rotation_Random_Four();

private:
	class CFreeCamera* m_Camera = nullptr;
	HEARTOFENDERSTATES m_pCurState = STATE_APPEAR;

	_int WalkCount = 0;
	_int FourSideCount = 0;
	_bool Splited[4] = { false,false,false,false };
	_bool m_IsHide = false;
	_float vScale = 1.f;

	_int PartObjSize = 0;
	vector<class CAttackBox*>		m_PartObjects;

	virtual void HurtSound() override;
	void VoiceSound();
	void FourSideSound();
	void SpitVoice();
	void StepVoice();

	_bool TeleSound[2] = { false,false };

	_float fBeamInterval = 0.f;
	_float fStepInterval = 0.f;

	void BeamLoop(_float fTimeDelta);
	void StepLoop(_float fTimeDelta);

public:
	static CHeartOfEnder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END