#pragma once
union Color
{
	struct
	{
		float r, g, b, a;
	};
	uint32_t data;
};

class IMaterial
{
};
struct Mesh
{
	std::vector<Eigen::Vector3f> mVertices;
	std::vector<Eigen::Vector3f> mNormals;
	std::vector<Eigen::Vector4f> mTangents;
	std::vector<std::vector<Eigen::Vector2f>> mTexCoordsArray;
	std::vector<uint32_t> mIndices;
	std::shared_ptr<IMaterial> mMaterial;
	Color mColor;
};

struct SkeletonMesh : public Mesh
{
	std::vector<Eigen::Vector3f> mWeights;
	std::vector<Eigen::Vector4i> mJoints;
};

struct Image
{
	uint32_t mWidth = 0;
	uint32_t mHeight = 0;
	uint32_t mChannels = 0;
	std::vector<float> mPixels;
};