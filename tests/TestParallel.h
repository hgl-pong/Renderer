#pragma once
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Common/Parallel.h"
using namespace Parallel;
TEST(ParallelAlgorithms, ParallelFill)
{
	std::vector<int> vec(1000, 0);
	ParallelFill(vec.begin(), vec.end(), 5);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ5
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 5; }));
}
TEST(ParallelAlgorithms, ParallelForSingle)
{
	std::vector<int> vec(1000, 0);
	ParallelFor<size_t, std::function<void(size_t)>>(0, vec.size(), [&](size_t i)
													 { vec[i] = 1; });
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ1
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 1; }));
}
TEST(ParallelAlgorithms, ParallelRangeForSingle)
{
	std::vector<int> vec(1000, 0);
	ParallelRangeFor<size_t, std::function<void(size_t, size_t)>>(0, vec.size(), [&](size_t begin, size_t end)
																  {
        for (size_t i = begin; i < end; ++i) {
            vec[i] = 2;
        } });
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ2
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 2; }));
}
TEST(ParallelAlgorithms, ParallelForDouble)
{
	const size_t width = 100, height = 100;
	std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

	ParallelFor<size_t, std::function<void(size_t, size_t)>>(0, width, 0, height, [&](size_t x, size_t y)
															 { matrix[y][x] = 3; });

	// ��֤�Ƿ�����Ԫ�ض�������Ϊ3
	for (const auto &row : matrix)
	{
		EXPECT_TRUE(std::all_of(row.begin(), row.end(), [](int i)
								{ return i == 3; }));
	}
}
TEST(ParallelAlgorithms, ParallelRangeForDouble)
{
	std::vector<std::vector<int>> matrix(100, std::vector<int>(100, 0));
	ParallelRangeFor<size_t, std::function<void(size_t, size_t, size_t, size_t)>>(0, matrix.size(), 0, matrix[0].size(), [&](size_t beginI, size_t endI, size_t beginJ, size_t endJ)
																				  {
        for (size_t i = beginI; i < endI; ++i) {
            for (size_t j = beginJ; j < endJ; ++j) {
                matrix[i][j] = 4;
            }
        } });
	// ��֤�Ƿ���������Ԫ�ض�������Ϊ4
	bool allFour = true;
	for (const auto &row : matrix)
	{
		if (!std::all_of(row.begin(), row.end(), [](int i)
						 { return i == 4; }))
		{
			allFour = false;
			break;
		}
	}
	EXPECT_TRUE(allFour);
}
TEST(ParallelAlgorithms, ParallelForWithPolicy)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ô��в���ִ��
	ParallelFor<size_t, std::function<void(size_t)>>(
		0, vec.size(), [&](size_t i)
		{ vec[i] = 5; },
		ExecutePolicy::eSequential);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ5
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 5; }));
}

TEST(ParallelAlgorithms, ParallelRangeForWithPolicy)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ô��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t)>>(
		0, vec.size(), [&](size_t begin, size_t end)
		{
        for (size_t i = begin; i < end; ++i) {
            vec[i] = 6;
        } },
		ExecutePolicy::eSequential);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ6
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 6; }));
}

TEST(ParallelAlgorithms, ParallelForDoubleWithPolicy)
{
	const size_t width = 100, height = 100;
	std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

	// ʹ�ô��в���ִ��
	ParallelFor<size_t, std::function<void(size_t, size_t)>>(
		0, width, 0, height, [&](size_t x, size_t y)
		{ matrix[y][x] = 7; },
		ExecutePolicy::eSequential);

	// ��֤�Ƿ�����Ԫ�ض�������Ϊ7
	for (const auto &row : matrix)
	{
		EXPECT_TRUE(std::all_of(row.begin(), row.end(), [](int i)
								{ return i == 7; }));
	}
}

TEST(ParallelAlgorithms, ParallelRangeForDoubleWithPolicy)
{
	std::vector<std::vector<int>> matrix(100, std::vector<int>(100, 0));
	// ʹ�ô��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t, size_t, size_t)>>(
		0, matrix.size(), 0, matrix[0].size(), [&](size_t beginI, size_t endI, size_t beginJ, size_t endJ)
		{
        for (size_t i = beginI; i < endI; ++i) {
            for (size_t j = beginJ; j < endJ; ++j) {
				matrix[i][j] = 8;
			}
		} },
		ExecutePolicy::eSequential);

	// ��֤�Ƿ���������Ԫ�ض�������Ϊ8
	bool allEight = true;
	for (const auto &row : matrix)
	{
		if (!std::all_of(row.begin(), row.end(), [](int i)
						 { return i == 8; }))
		{
			allEight = false;
			break;
		}
	}
	EXPECT_TRUE(allEight);
}

TEST(ParallelAlgorithms, ParallelForWithPolicyParallel)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ò��в���ִ��
	ParallelFor<size_t, std::function<void(size_t)>>(
		0, vec.size(), [&](size_t i)
		{ vec[i] = 9; },
		ExecutePolicy::eParallel);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ9
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 9; }));
}

TEST(ParallelAlgorithms, ParallelRangeForWithPolicyParallel)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ò��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t)>>(
		0, vec.size(), [&](size_t begin, size_t end)
		{
        for (size_t i = begin; i < end; ++i) {
			vec[i] = 10;
		} },
		ExecutePolicy::eParallel);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ10
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 10; }));
}

TEST(ParallelAlgorithms, ParallelForDoubleWithPolicyParallel)
{
	const size_t width = 100, height = 100;
	std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

	// ʹ�ò��в���ִ��
	ParallelFor<size_t, std::function<void(size_t, size_t)>>(
		0, width, 0, height, [&](size_t x, size_t y)
		{ matrix[y][x] = 11; },
		ExecutePolicy::eParallel);

	// ��֤�Ƿ�����Ԫ�ض�������Ϊ11
	for (const auto &row : matrix)
	{
		EXPECT_TRUE(std::all_of(row.begin(), row.end(), [](int i)
								{ return i == 11; }));
	}
}

TEST(ParallelAlgorithms, ParallelRangeForDoubleWithPolicyParallel)
{
	std::vector<std::vector<int>> matrix(100, std::vector<int>(100, 0));
	// ʹ�ò��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t, size_t, size_t)>>(
		0, matrix.size(), 0, matrix[0].size(), [&](size_t beginI, size_t endI, size_t beginJ, size_t endJ)
		{
        for (size_t i = beginI; i < endI; ++i) {
            for (size_t j = beginJ; j < endJ; ++j) {
				matrix[i][j] = 12;
			}
		} },
		ExecutePolicy::eParallel);

	// ��֤�Ƿ���������Ԫ�ض�������Ϊ12
	bool allTwelve = true;
	for (const auto &row : matrix)
	{
		if (!std::all_of(row.begin(), row.end(), [](int i)
						 { return i == 12; }))
		{
			allTwelve = false;
			break;
		}
	}
	EXPECT_TRUE(allTwelve);
}

TEST(ParallelAlgorithms, ParallelForWithPolicySingle)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ô��в���ִ��
	ParallelFor<size_t, std::function<void(size_t)>>(
		0, vec.size(), [&](size_t i)
		{ vec[i] = 13; },
		ExecutePolicy::eSequential);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ13
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 13; }));
}

TEST(ParallelAlgorithms, ParallelRangeForWithPolicySingle)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ô��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t)>>(
		0, vec.size(), [&](size_t begin, size_t end)
		{
        for (size_t i = begin; i < end; ++i) {
			vec[i] = 14;
		} },
		ExecutePolicy::eSequential);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ14
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 14; }));
}

TEST(ParallelAlgorithms, ParallelForDoubleWithPolicySingle)
{
	const size_t width = 100, height = 100;
	std::vector<std::vector<int>> matrix(height, std::vector<int>(width, 0));

	// ʹ�ô��в���ִ��
	ParallelFor<size_t, std::function<void(size_t, size_t)>>(
		0, width, 0, height, [&](size_t x, size_t y)
		{ matrix[y][x] = 15; },
		ExecutePolicy::eSequential);

	// ��֤�Ƿ�����Ԫ�ض�������Ϊ15
	for (const auto &row : matrix)
	{
		EXPECT_TRUE(std::all_of(row.begin(), row.end(), [](int i)
								{ return i == 15; }));
	}
}

TEST(ParallelAlgorithms, ParallelRangeForDoubleWithPolicySingle)
{
	std::vector<std::vector<int>> matrix(100, std::vector<int>(100, 0));
	// ʹ�ô��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t, size_t, size_t)>>(
		0, matrix.size(), 0, matrix[0].size(), [&](size_t beginI, size_t endI, size_t beginJ, size_t endJ)
		{
        for (size_t i = beginI; i < endI; ++i) {
            for (size_t j = beginJ; j < endJ; ++j) {
				matrix[i][j] = 16;
			}
		} },
		ExecutePolicy::eSequential);

	// ��֤�Ƿ���������Ԫ�ض�������Ϊ16
	bool allSixteen = true;
	for (const auto &row : matrix)
	{
		if (!std::all_of(row.begin(), row.end(), [](int i)
						 { return i == 16; }))
		{
			allSixteen = false;
			break;
		}
	}
	EXPECT_TRUE(allSixteen);
}

TEST(ParallelAlgorithms, ParallelForWithPolicyParallelSingle)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ò��в���ִ��
	ParallelFor<size_t, std::function<void(size_t)>>(
		0, vec.size(), [&](size_t i)
		{ vec[i] = 17; },
		ExecutePolicy::eParallel);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ17
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 17; }));
}

TEST(ParallelAlgorithms, ParallelRangeForWithPolicyParallelSingle)
{
	std::vector<int> vec(1000, 0);
	// ʹ�ò��в���ִ��
	ParallelRangeFor<size_t, std::function<void(size_t, size_t)>>(
		0, vec.size(), [&](size_t begin, size_t end)
		{
		for (size_t i = begin; i < end; ++i) {
			vec[i] = 18;
		} },
		ExecutePolicy::eParallel);
	// ��֤�Ƿ�����Ԫ�ض�������Ϊ18
	EXPECT_TRUE(std::all_of(vec.begin(), vec.end(), [](int i)
							{ return i == 18; }));
}

TEST(ParallelAlgorithms, ParallelSortWithPolicy)
{
	std::vector<int> vec(1000);
	std::generate(vec.begin(), vec.end(), []()
				  { return rand() % 1000; });
	std::vector<int> sortedVec = vec;
	std::sort(sortedVec.begin(), sortedVec.end());
	ParallelSort<std::vector<int>::iterator, std::less<int>>(vec.begin(), vec.end());
	EXPECT_EQ(vec, sortedVec);
}

TEST(ParallelAlgorithms, ParallelReduceWithPolicy)
{
	std::vector<int> vec(1000);
	std::generate(vec.begin(), vec.end(), []()
				  { return rand() % 1000; });
	int sum = std::accumulate(vec.begin(), vec.end(), 0);
	int parallelSum = ParallelReduce<size_t, int>(
		0, vec.size(), 0, [&](size_t begin, size_t end, int init)
		{ return std::accumulate(vec.begin() + begin, vec.begin() + end, init); },
		std::plus<int>());
	EXPECT_EQ(sum, parallelSum);
}