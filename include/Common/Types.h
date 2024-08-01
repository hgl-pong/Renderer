#pragma once
#include "pch.h"
#include <memory>

template <typename T>
using UniquePtr = std::unique_ptr<T>;
template <typename T>
using SharedPtr = std::shared_ptr<T>;
template <typename T>
using WeakPtr = std::weak_ptr<T>;

template <typename T, typename... Args>
UniquePtr<T> MakeUniquePtr(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename... Args>
SharedPtr<T> MakeSharedPtr(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T>
WeakPtr<T> MakeWeakPtr(const SharedPtr<T>& ptr)
{
	return WeakPtr<T>(ptr);
}

union Color
{
	struct
	{
		float r, g, b, a;
	};
	uint32_t data;
};

enum class AssetType
{
	IMAGE,
	MODEL,
	MATERIAL
};

class IMaterial
{
};

struct Mesh
{
	std::vector<MathLib::HVector3> mVertices;
	std::vector<MathLib::HVector3> mNormals;
	std::vector<MathLib::HVector4> mTangents;
	std::vector<std::vector<MathLib::HVector2>> mTexCoordsArray;
	std::vector<uint32_t> mIndices;
	std::shared_ptr<IMaterial> mMaterial;
	// BoundingBox mBoundingBox;
	Color mColor;
};

struct SkeletonMesh : public Mesh
{
	std::vector<MathLib::HVector3> mWeights;
	std::vector<MathLib::HVector4> mJoints;
};

struct Image
{
	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	uint32_t mChannels = 0;
	std::vector<float> mPixels;
};