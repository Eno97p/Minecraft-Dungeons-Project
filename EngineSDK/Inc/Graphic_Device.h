#pragma once

#include "Base.h"

BEGIN(Engine)

/* 1. ID3D11Device, ID3D11DeviceContext��� ��ü�� �켱 �����Ѵ�. */
/* 2. IDXGISwapChain�� �����Ѵ�.(�����(ID3D11Texture2D)�� ���� �����Ѱž�.) */
/* 3. ����ۺ並 �����Ѵ�. */
/* 4. ���̹��� �ؽ��ĸ� �����ϰ� ���̹��� �並 �����Ѵ�. */

class CGraphic_Device final : public CBase
{
public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;

public:

	/* �׷��� ����̽��� �ʱ�ȭ. */
	HRESULT Initialize(const ENGINE_DESC& EngineDesc,
		_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext** ppContext);

	/* ����۸� �����. */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Create_AnimDepthStencil();
	HRESULT Create_TerrainDepthStencil();


	/* ���̹��� + ���ٽǹ��۸� �����. */
	HRESULT Clear_DepthStencil_View();

	/* �ĸ� ���۸� ������۷� ��ü�Ѵ�.(����۸� ȭ�鿡 ���� �����ش�.) */
	HRESULT Present();

private:
	// IDirect3DDevice9* == LPDIRECT3DDEVICE9 == ID3D11Device + ID3D11DeviceContext 

	/* �޸� �Ҵ�. (��������, �ε�������, �ؽ��ķε�) �İ�ü�� ������ ���õ� ���� */
	ID3D11Device* m_pDevice = { nullptr };

	/* ��ɽ���.(���ε��۾�, �������۸� SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	/* �׸���. */
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };

	/* �ĸ���ۿ� ������۸� ��ü�ذ��鼭 ȭ�鿡 �����ִ� ���� */
	IDXGISwapChain* m_pSwapChain = { nullptr };


	/* IDirect3DTexture9 */
	/* ID3D11Texture2D : �ؽ��ĸ� ǥ���ϴ� ������ü Ÿ���̴�.
	why? �뵵�� �´� ���������� ����ϱ� ���� �ؽ��İ�ü�� ����������.  */

	/* ID3D11ShaderResourceView : ���̴��� ���޵� �� �ִ� �ؽ�ó Ÿ��. */
	/* ID3D11RenderTargetView : ����Ÿ�ٿ����� ���� �� �ִ� �ؽ�ó Ÿ��. */
	/* ID3D11DepthStencilView : ���̽��ٽ� ���۷μ� ���� �� �ִ� Ÿ��.  */
	ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
	ID3D11Texture2D* m_pDepthTexture = { nullptr };

	ID3D11DepthStencilView* m_pDepthStencilView = { nullptr };

private:
	/* ����ü�ο��� �ʼ������� �ʿ��� �����ʹ� ����۰� �ʿ��Ͽ� ����۸� �����ϱ����� ������ �����ش�. */
	/* ����ü���� ������� == �����(�ؽ���)�� �����ȴ�. */
	HRESULT Ready_SwapChain(HWND hWnd, _bool isWindowed, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);

public:
	static CGraphic_Device* Create(const ENGINE_DESC& EngineDesc, _Out_ ID3D11Device** ppDevice,
		_Out_ ID3D11DeviceContext** ppDeviceContextOut);
	virtual void Free() override;
};

END