#include "Value.hpp"
#include "Memory.hpp"
#include "Array.hpp"
std::string convertValueToString(Value const &val)
{
    switch (val.index())
    {
    case ValueType::Integer:
        return std::to_string(std::get<int64_t>(val));
    case ValueType::String:
        return std::get<StringNode *>(val)->getValue();
    case ValueType::Array:
        return std::get<ArrayNode *>(val)->toString();
    default:
        throw std::runtime_error("Rest of value handling not implemented");
    }
}

bool isValueNull(Value const &val)
{
    switch (val.index())
    {
    case ValueType::Integer:
        return std::get<int64_t>(val) == 0;
    case ValueType::String:
        return std::get<StringNode *>(val)->getValue() == "";
    case ValueType::Array:
        return std::get<ArrayNode *>(val)->isEmpty();
    default:
        throw std::runtime_error("Rest of value null handling not implemented");
    }
}

void increaseValueRefCount(Value const &val)
{
    switch (val.index())
    {
    case ValueType::String:
        std::get<StringNode *>(val)->increaseRefCount();
        break;
    case ValueType::Array:
        std::get<ArrayNode *>(val)->increaseRefCount();
        break;
    }
}

void decreaseValueRefCount(Value const &val)
{
    switch (val.index())
    {
    case ValueType::String:
        std::get<StringNode *>(val)->decreaseRefCount();
        break;
    case ValueType::Array:
        std::get<ArrayNode *>(val)->decreaseRefCount();
        break;
    }
}

bool areValuesTheSame(Value const &a, Value const &b)
{
    if (isValueNull(a) && isValueNull(b))
    {
        return true;
    }

    if (a.index() == ValueType::Integer && b.index() == ValueType::Integer)
    {
        return std::get<int64_t>(a) == std::get<int64_t>(b);
    }
    else if (a.index() == ValueType::String && b.index() == ValueType::String)
    {
        return std::get<StringNode *>(a)->getValue() == std::get<StringNode *>(b)->getValue();
    }
    else if (a.index() == ValueType::Array && b.index() == ValueType::Array)
    {

        return std::get<ArrayNode *>(a)->equalTo(std::get<ArrayNode *>(b));
    }

    else if (a.index() == ValueType::Integer && b.index() == ValueType::String)
    {
        return std::to_string(std::get<int64_t>(a)) == std::get<StringNode *>(b)->getValue();
    }

    else if (a.index() == ValueType::String && b.index() == ValueType::Integer)
    {
        return std::get<StringNode *>(a)->getValue() == std::to_string(std::get<int64_t>(b));
    }

    return false;
}

bool areValuesEqual(Value const &a, Value const &b)
{
    if (a.index() != b.index())
    {
        return false;
    }
    switch (a.index())
    {
    case ValueType::Integer:
        return std::get<int64_t>(a) == std::get<int64_t>(b);
    case ValueType::String:
        return std::get<StringNode *>(a) == std::get<StringNode *>(b);
    case ValueType::Array:
        return std::get<ArrayNode *>(a) == std::get<ArrayNode *>(b);
    }
    // should not be reachable but exists in case of future changes
    return false;
}