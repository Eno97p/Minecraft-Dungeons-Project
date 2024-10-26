#pragma once

#include "Base.h"

/* �ִԿ��Ӽ��̤� ����ϴ� ���� �ϳ��� ����. */
/* ���� : �ð��� ���� �� ���� ����(��, ��, ��)���� ����̤����Ѵ�. == Ű������ */
/* */

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim*	pAIChannel, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(_double CurrentPosition, const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
	void Lerp_TransformationMatrix(const vector<class CBone*>& Bones, class CChannel* pNextChannel, _double LerpTime, _double LerpTimeAcc, _uint iCurrentKeyFrameIndex);
	void Reset_TransformationMatrix(const vector<class CBone*>& Bones, _uint* pCurrentKeyFrameIndex);
private:
	_char			m_szName[MAX_PATH] = "";
	_uint			m_iBoneIndex = { 0 };

	_uint				m_iNumKeyFrames = { 0 };
	vector<KEYFRAME>	m_KeyFrames;

public:
	HRESULT Save_Channel(ostream& os);
	HRESULT Load_Channel(istream& is);
	static CChannel* LoadCreate(istream& is);
	static CChannel* Create(const aiNodeAnim*	pAIChannel, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END