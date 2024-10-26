#pragma once
#include "Engine_Defines.h"

namespace Engine
{

    inline float RandomFloat(float min, float max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    inline int RandomInt(int min, int max) 
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }


    inline int RandomSign() //-1 Ȥ�� 1�� ��ȯ����
    {
        float randomNumber = RandomFloat(-1.0f, 1.0f);
        if (randomNumber < 0)
            return -1;
        else
            return 1;
    }

    inline _float4 RandomUnitVector() {
        float theta = RandomFloat(0.0f, 2.0f * M_PI); // 0�� 2�� ������ ������ ����
        float phi = acos(RandomFloat(-1.0f, 1.0f)); // -1�� 1 ������ ������ ���� ��ũ �ڻ���

        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);

        // ���͸� ����ȭ
        float length = sqrt(x * x + y * y + z * z);
        if (length != 0) {
            x /= length;
            y /= length;
            z /= length;
        }

        return _float4(x, y, z, 0.0f);
    }

    inline _float LerpFloat(_float fStart, _float fEnd, _float _t)
    {
        XMVECTOR startVec = XMVectorSet(fStart, 0.0f, 0.0f, 0.0f);
        XMVECTOR endVec = XMVectorSet(fEnd, 0.0f, 0.0f, 0.0f);
        XMVECTOR resultVec = XMVectorLerp(startVec, endVec, _t);
        _float result;
        XMStoreFloat(&result, resultVec);
        return result;
    }


   inline float CalculateAngleBetweenVectors(const XMVECTOR& v1, const XMVECTOR& v2)
    {
        float dotProduct = XMVectorGetX(XMVector3Dot(v1, v2));
        float v1Length = XMVectorGetX(XMVector3Length(v1));
        float v2Length = XMVectorGetX(XMVector3Length(v2));

        // ������ ���̰� 0�̸� ������ ���ǵ��� ����
        if (v1Length == 0 || v2Length == 0)
            return 0;

        // �� ������ ������ �̿��Ͽ� ���� ���
        float cosTheta = dotProduct / (v1Length * v2Length);
        float angle = std::acos(cosTheta);

        // ������ ������ ��ȯ�Ͽ� ��ȯ
        return XMConvertToDegrees(angle);
    }

   inline float GetDirectionAngle(const XMVECTOR& baseDirection)
    {
        // ���� ���Ϳ� x�� ������ ������ ���
        XMVECTOR xAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        float angle = CalculateAngleBetweenVectors(baseDirection, xAxis);

        return angle;
    }

}