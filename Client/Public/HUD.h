#pragma once

#include "Client_Defines.h"
#include "Player.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CHUD final : public CUI
{
public:
	typedef struct HUDDESC : public UIDESC
	{
		CPlayer::PLAYERSTATUS* pPlayerStat;
	};

private:
	CHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHUD(const CHUD& rhs);
	virtual ~CHUD() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Tick(_float fTimeDelta) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

	_float  m_pRatio = 0.f;
	_float m_PreHP = 0.f;
	_float m_ResetTime = 0.f;

	CPlayer::PLAYERSTATUS* m_PlayerStatus = nullptr;

public:
	static CHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END