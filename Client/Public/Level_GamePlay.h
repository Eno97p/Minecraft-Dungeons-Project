#pragma once

#include "Client_Defines.h"
#include "Level.h"
BEGIN(Engine)
class CGameObject;
END
BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;


private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_Mouse(const wstring& strLayerTag);
	HRESULT Ready_Layer_Player(const wstring& strLayerTag);
	HRESULT Ready_Layer_Monster();
	HRESULT Ready_Layer_Map(const wstring& strLayerTag);
	HRESULT Camera_Setting();
	HRESULT Ready_Lights();
private:
	_float m_fCutSceneTime = 1.f;
	_float AccTime = 0.f;
	_bool BgmOn = false;
	_bool m_fStarted = false;
	CGameObject* m_pCamera = nullptr;
	CGameObject* m_pPlayer = nullptr;
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END