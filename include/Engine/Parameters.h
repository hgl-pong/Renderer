#pragma once
#include "Common/pch.h"

template <typename DataType>
class RenderParameter
{
public:
    RenderParameter(const std::string &name, const DataType &defaultValue)
        : m_Name(name), m_DefaultValue(defaultValue), m_Value(defaultValue)
    {
    }

    void SetValue(const DataType &value)
    {
        m_Value = value;
    }

    const DataType &GetValue() const
    {
        return m_Value;
    }

    const DataType &GetDefaultValue() const
    {
        return m_DefaultValue;
    }

    const std::string &GetName() const
    {
        return m_Name;
    }

    bool operator==(const RenderParameter<DataType> &other) const
    {
        return m_Name == other.m_Name;
    }

    bool operator!=(const RenderParameter<DataType> &other) const
    {
        return m_Name != other.m_Name;
    }

    bool operator<(const RenderParameter<DataType> &other) const
    {
        return m_Name < other.m_Name;
    }

    bool operator>(const RenderParameter<DataType> &other) const
    {
        return m_Name > other.m_Name;
    }

    bool operator<=(const RenderParameter<DataType> &other) const
    {
        return m_Name <= other.m_Name;
    }

    bool operator>=(const RenderParameter<DataType> &other) const
    {
        return m_Name >= other.m_Name;
    }

    bool operator==(const std::string &name) const
    {
        return m_Name == name;
    }

    bool operator!=(const std::string &name) const
    {
        return m_Name != name;
    }

    bool operator<(const std::string &name) const
    {
        return m_Name < name;
    }

    bool operator>(const std::string &name) const
    {
        return m_Name > name;
    }

    bool operator<=(const std::string &name) const
    {
        return m_Name <= name;
    }

    bool operator>=(const std::string &name) const
    {
        return m_Name >= name;
    }

    bool operator==(const DataType &value) const
    {
        return m_Value == value;
    }

    bool operator!=(const DataType &value) const
    {
        return m_Value != value;
    }

    bool operator<(const DataType &value) const
    {
        return m_Value < value;
    }

    bool operator>(const DataType &value) const
    {
        return m_Value > value;
    }

private:
    std::string m_Name;
    DataType m_DefaultValue;
    DataType m_Value;
};