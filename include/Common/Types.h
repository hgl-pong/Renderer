#pragma once
#include "pch.h"
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