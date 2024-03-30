#pragma once
#include "pch.h"
#define USE_TBB
#ifdef USE_TBB
#include <tbb/task.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>
#include <tbb/parallel_reduce.h>
#endif

namespace Parallel
{
#ifdef USE_TBB
    enum class ExecutePolicy
    {
        eSequential,
        eParallel,
    };

    template <typename IndexType, typename Function>
    void ParallelFor(IndexType beginIndex, IndexType endIndex,
                     const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        if (beginIndex > endIndex)
            return;
        if (policy == ExecutePolicy::eParallel)
            tbb::parallel_for(beginIndex, endIndex, function);
        else
        {
            for (auto i = beginIndex; i < endIndex; ++i)
                function(i);
        }
    };

    template <typename IndexType, typename Function>
    void ParallelRangeFor(IndexType beginIndex, IndexType endIndex,
                          const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        if (beginIndex > endIndex)
            return;

        if (policy == ExecutePolicy::eParallel)
            tbb::parallel_for(tbb::blocked_range<IndexType>(beginIndex, endIndex),
                              [&function](const tbb::blocked_range<IndexType> &range)
                              {
                                  function(range.begin(), range.end());
                              });
        else
            function(beginIndex, endIndex);
    }

    template <typename IndexType, typename Function>
    void ParallelFor(IndexType beginIndexX, IndexType endIndexX,
                     IndexType beginIndexY, IndexType endIndexY,
                     const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        ParallelFor<IndexType, std::function<void(IndexType)>>(
            beginIndexY, endIndexY,
            [&](IndexType j)
            {
			for (IndexType i = beginIndexX; i < endIndexX; ++i) 
				function(i, j); },
            policy);
    }

    template <typename IndexType, typename Function>
    void ParallelRangeFor(IndexType beginIndexX, IndexType endIndexX,
                          IndexType beginIndexY, IndexType endIndexY,
                          const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        ParallelRangeFor<IndexType, std::function<void(IndexType,IndexType)>>(
            beginIndexY, endIndexY,
            [&](IndexType jBegin, IndexType jEnd)
            { function(beginIndexX, endIndexX, jBegin, jEnd); },
            policy);
    }

    template <typename IndexType, typename Function>
    void ParallelFor(IndexType beginIndexX, IndexType endIndexX,
                     IndexType beginIndexY, IndexType endIndexY,
                     IndexType beginIndexZ, IndexType endIndexZ,
                     const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        ParallelFor<IndexType, std::function<void(IndexType)>>(
            beginIndexZ, endIndexZ,
            [&](IndexType k)
            {
			for (IndexType j = beginIndexY; j < endIndexY; ++j) 
				for (IndexType i = beginIndexX; i < endIndexX; ++i) 
					function(i, j, k); },
            policy);
    }

    template <typename IndexType, typename Function>
    void ParallelRangeFor(IndexType beginIndexX, IndexType endIndexX,
                          IndexType beginIndexY, IndexType endIndexY,
                          IndexType beginIndexZ, IndexType endIndexZ,
                          const Function &function, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        ParallelRangeFor(
            beginIndexZ, endIndexZ,
            [&](IndexType kBegin, IndexType kEnd)
            { function(beginIndexX, endIndexX, beginIndexY,
                       endIndexY, kBegin, kEnd); },
            policy);
    }

    template <typename IndexType, typename Value, typename Function, typename Reduce>
    Value ParallelReduce(IndexType beginIndex, IndexType endIndex,
                         const Value &identity, const Function &func,
                         const Reduce &reduce, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        if (beginIndex > endIndex)
            return identity;
        if (policy == ExecutePolicy::eParallel)
        {
            return tbb::parallel_reduce(
                tbb::blocked_range<IndexType>(beginIndex, endIndex), identity,
                [&func](const tbb::blocked_range<IndexType> &range, const Value &init)
                { return func(range.begin(), range.end(), init); },
                reduce);
        }
        else
        {
            (void)reduce;
            return func(beginIndex, endIndex, identity);
        }
    }

    template <typename RandomIterator, typename T>
    void ParallelFill(const RandomIterator& begin, const RandomIterator& end,
        const T& value, ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        auto diff = end - begin;
        if (diff <= 0)
            return;
        size_t size = static_cast<size_t>(diff);
        ParallelFor<size_t,std::function<void(size_t)>>(
            0, size, [begin, value](size_t i)
            { begin[i] = value; },
            policy);
    };

    template <typename RandomIterator, typename CompareFunction>
    void ParallelSort(RandomIterator begin, RandomIterator end, CompareFunction compareFunction = std::less<typename std::iterator_traits<RandomIterator>::value_type>(),
                      ExecutePolicy policy = ExecutePolicy::eParallel)
    {
        if (end < begin)
            return;
        if (policy == ExecutePolicy::eParallel)
            tbb::parallel_sort(begin, end, compareFunction);
        else
            std::sort(begin, end, compareFunction);
    }

#endif // USE_TBB
} // namespace Parallel
