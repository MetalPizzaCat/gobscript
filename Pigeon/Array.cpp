#include "Array.hpp"

ArrayNode::ArrayNode(std::vector<Value> const &values) : m_values(values)
{
    for (Value const &v : m_values)
    {
        increaseValueRefCount(v);
    }
}

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
        return;
    }
    throw RuntimeActionExecutionError("Attempted to access out of bounds value in the array");
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

void ArrayNode::pushBack(Value const &val)
{
    if (val.index() != ValueType::Array || getValueAsArray(val) != this)
    {
        increaseValueRefCount(val);
    }
    m_values.push_back(val);
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
