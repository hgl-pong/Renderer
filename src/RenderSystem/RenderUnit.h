#pragma once
#include "Common/pch.h"
#include "Engine/RenderSystemInterface.h"

class RenderUnit : virtual public IRenderUnit
{
public:
    RenderUnit() {}
    virtual ~RenderUnit() {}
    void SetRenderOrder(uint32_t order) override { m_RenderOrder = order; }
    uint32_t GetRenderOrder() const override { return m_RenderOrder; }
    void Render() override {}

private:
    friend bool CompareWithRenderOrder(const SharedPtr<RenderUnit> &a, const SharedPtr<RenderUnit> &b);

private:
    uint32_t m_RenderOrder = 0;
};

inline bool CompareWithRenderOrder(const SharedPtr<RenderUnit> &a, const SharedPtr<RenderUnit> &b)
{
    return a->m_RenderOrder < b->m_RenderOrder;
}
class RenderQueue
{
public:
    void AddRenderUnit(SharedPtr<RenderUnit> renderUnit)
    {
        m_RenderUnits.push_back(renderUnit);
    }

    void Clear()
    {
        m_RenderUnits.clear();
    }

    void Render()
    {
        for (auto &unit : m_RenderUnits)
        {
            unit->Render();
        }
    }

    void SortByRenderOrder()
    {
        std::sort(m_RenderUnits.begin(), m_RenderUnits.end(), CompareWithRenderOrder);
    }

private:
    std::vector<SharedPtr<RenderUnit>> m_RenderUnits;
};
