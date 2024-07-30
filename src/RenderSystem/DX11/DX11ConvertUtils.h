#pragma once

#include <Math/Math.h>
#include <DirectXMath.h>

namespace DX11ConvertUtils
{
	inline MathLib::HVector2 FromDX(const DirectX::XMFLOAT2 &vec)
	{
		return MathLib::HVector2(vec.x, vec.y);
	}

	inline DirectX::XMFLOAT2 ToDX(const MathLib::HVector2 &vec)
	{
		return DirectX::XMFLOAT2(vec[0], vec[1]);
	}

	inline MathLib::HVector3 FromDX(const DirectX::XMFLOAT3 &vec)
	{
		return MathLib::HVector3(vec.x, vec.y, vec.z);
	}

	inline DirectX::XMFLOAT3 ToDX(const MathLib::HVector3 &vec)
	{
		return DirectX::XMFLOAT3(vec[0], vec[1], vec[2]);
	}

	inline MathLib::HVector4 FromDX(const DirectX::XMFLOAT4 &vec)
	{
		return MathLib::HVector4(vec.x, vec.y, vec.z, vec.w);
	}

	inline DirectX::XMFLOAT4 ToDX(const MathLib::HVector4 &vec)
	{
		return DirectX::XMFLOAT4(vec[0], vec[1], vec[2], vec[3]);
	}

	inline MathLib::HMatrix4 FromDX(const DirectX::XMFLOAT4X4 &mat)
	{
		MathLib::HMatrix4 result;
		result<< mat(0, 0), mat(0, 1), mat(0, 2), mat(0, 3),
			 mat(1, 0), mat(1, 1), mat(1, 2), mat(1, 3),
            mat(2, 0), mat(2, 1), mat(2, 2), mat(2, 3),
			 mat(3, 0), mat(3, 1), mat(3, 2), mat(3, 3);
		return result;
	}
}