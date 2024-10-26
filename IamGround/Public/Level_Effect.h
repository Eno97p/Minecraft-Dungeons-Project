#pragma once

#include "Tool_Define.h"
#include "Level.h"

BEGIN(TOOL)

class CLevel_Effect final : public CLevel
{
private:
	CLevel_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Effect() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_float fTimeDelta) override;


private:
	HRESULT Ready_Layer_Camera(const wstring& strLayerTag);
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);

	HRESULT Ready_Lights();
public:
	static CLevel_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END