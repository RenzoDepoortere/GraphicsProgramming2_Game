#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

namespace MathHelper
{
	inline XMFLOAT3 QuaternionToEuler(XMFLOAT4 q)
	{
		XMFLOAT3 euler{};

		euler.y = atan2f(2.f * q.x * q.w + 2.f * q.y * q.z, 1.f - 2.f * (q.z * q.z + q.w * q.w));     // Yaw 
		euler.x = asinf(2.f * (q.x * q.z - q.w * q.y));                             // Pitch 
		euler.z = atan2f(2.f * q.x * q.y + 2.f * q.z * q.w, 1.f - 2.f * (q.y * q.y + q.z * q.z));      // Roll 

		return euler;
	}

	inline bool XMFloat4Equals(const XMFLOAT4& a, const XMFLOAT4& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
	}

	inline bool XMFloat3Equals(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}

	inline bool XMFloat2Equals(const XMFLOAT2& a, const XMFLOAT2& b)
	{
		return a.x == b.x && a.y == b.y;
	}

	inline XMFLOAT3 DirectionTo(const XMFLOAT3& from, const XMFLOAT3& to, bool normalize = true)
	{
		const XMVECTOR fromVector{ XMLoadFloat3(&from) };
		const XMVECTOR toVector{ XMLoadFloat3(&to) };

		const XMVECTOR desiredVector = (normalize) ? XMVector3Normalize(XMVectorSubtract(toVector, fromVector)) : XMVectorSubtract(toVector, fromVector);
		XMFLOAT3 desiredDirection{};
		XMStoreFloat3(&desiredDirection, desiredVector);

		return desiredDirection;
	}

	inline float toDegrees(float angle)
	{
		return angle * static_cast<float>(180.f / M_PI);
	}
	inline float toRadians(float angle)
	{
		return angle * static_cast<float>(M_PI / 180.f);
	}

	inline float randF(float min, float max)
	{
		const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		const float diff = max - min;
		const float r = random * diff;
		return min + r;
	}

	inline LONG BinaryClamp(LONG x)
	{
		if (x > 0) return 1;
		if (x < 0) return -1;
		return 0;
	}

	template<typename T>
	inline T Lerp(T a, T b, float t)
	{
		return (1 - t) * a + t * b;
	}

	template<typename T>
	void Clamp(T& value, T hi, T lo)
	{
		if (value > hi)
			value = hi;

		if (value < lo)
			value = lo;
	}
}