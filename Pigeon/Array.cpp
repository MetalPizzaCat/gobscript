#include "Array.hpp"

ArrayNode::ArrayNode(std::vector<Value> const &values) : m_values(values) {}

std::optional<Value> ArrayNode::getValuesAt(size_t i) const
{
    if (i < m_values.size())
    {
        return m_values[i];
    }
    return {};
}

void ArrayNode::setValue(size_t i, Value val)
{
    if (i < m_values.size())
    {
        m_values[i] = val;
    }
}

std::string ArrayNode::toString() const
{
    std::string temp = "[";
    for (size_t i = 0; i < m_values.size(); i++)
    {
        temp += convertValueToString(m_values[i]);
        if (i < m_values.size() - 1)
        {
            temp += ',';
        }
    }
    return temp + "]";
}
