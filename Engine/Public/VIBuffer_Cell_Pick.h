#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cell_Pick final : public CVIBuffer
{
private:
	CVIBuffer_Cell_Pick(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cell_Pick(const CVIBuffer_Cell_Pick& rhs);
	virtual ~CVIBuffer_Cell_Pick() = default;

public:
	virtual HRESULT Initialize_Prototype(const _float3* pPositions);
	virtual HRESULT Initialize(void* pArg) override;

public:
	static CVIBuffer_Cell_Pick* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPositions);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END