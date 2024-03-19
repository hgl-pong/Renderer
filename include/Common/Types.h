#pragma once

typedef Eigen::Vector2f Vector2f;
typedef Eigen::Vector3f Vector3f;
typedef Eigen::Vector4f Vector4f;
typedef Eigen::Vector2i Vector2i;
typedef Eigen::Vector3i Vector3i;
typedef Eigen::Vector4i Vector4i;
typedef Eigen::Matrix4f Matrix4f;
typedef Eigen::Matrix3f Matrix3f;
typedef Eigen::Quaternionf Quaternionf;
typedef Eigen::Affine3f Affine3f;
typedef Eigen::AlignedBox3f BoundingBox;
typedef Eigen::AlignedBox2f BoundingBox2D;

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
	std::vector<Vector3f> mVertices;
	std::vector<Vector3f> mNormals;
	std::vector<Vector4f> mTangents;
	std::vector<std::vector<Vector2f>> mTexCoordsArray;
	std::vector<uint32_t> mIndices;
	std::shared_ptr<IMaterial> mMaterial;
	BoundingBox mBoundingBox;
	Color mColor;
};

struct SkeletonMesh : public Mesh
{
	std::vector<Vector3f> mWeights;
	std::vector<Vector4i> mJoints;
};

struct Image
{
	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	uint32_t mChannels = 0;
	std::vector<float> mPixels;
};