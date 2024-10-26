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


    inline int RandomSign() //-1 혹은 1을 반환해줌
    {
        float randomNumber = RandomFloat(-1.0f, 1.0f);
        if (randomNumber < 0)
            return -1;
        else
            return 1;
    }

    inline _float4 RandomUnitVector() {
        float theta = RandomFloat(0.0f, 2.0f * M_PI); // 0과 2π 사이의 랜덤한 각도
        float phi = acos(RandomFloat(-1.0f, 1.0f)); // -1과 1 사이의 랜덤한 값의 아크 코사인

        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);

        // 벡터를 정규화
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

        // 벡터의 길이가 0이면 각도는 정의되지 않음
        if (v1Length == 0 || v2Length == 0)
            return 0;

        // 두 벡터의 내적을 이용하여 각도 계산
        float cosTheta = dotProduct / (v1Length * v2Length);
        float angle = std::acos(cosTheta);

        // 라디안을 각도로 변환하여 반환
        return XMConvertToDegrees(angle);
    }

   inline float GetDirectionAngle(const XMVECTOR& baseDirection)
    {
        // 기준 벡터와 x축 사이의 각도를 계산
        XMVECTOR xAxis = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        float angle = CalculateAngleBetweenVectors(baseDirection, xAxis);

        return angle;
    }

}