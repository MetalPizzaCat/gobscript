#include "Array.hpp"

ArrayNode::ArrayNode(std::vector<Value> const &values) : m_values(values) {}

std::optional<Value> ArrayNode::getValueAt(size_t i) const
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

bool ArrayNode::equalTo(ArrayNode const *other)
{
    if (m_values.size() != other->m_values.size())
    {
        return false;
    }
    for (size_t i = 0; i < m_values.size(); i++)
    {
        if (!areValuesTheSame(m_values[i], other->m_values[i]))
        {
            return false;
        }
    }
    return true;
}

ArrayNode::~ArrayNode()
{
    for (size_t i = 0; i < m_values.size(); i++)
    {
        if (m_values[i].index() == ValueType::Array)
        {
            std::get<ArrayNode *>(m_values[i])->decreaseRefCount();
        }
        else if (m_values[i].index() == ValueType::String)
        {
            std::get<StringNode *>(m_values[i])->decreaseRefCount();
        }
    }
}
