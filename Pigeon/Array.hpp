#pragma once
#include "Value.hpp"
#include "Memory.hpp"
#include <vector>
#include <optional>

class ArrayNode : public MemoryNode
{
public:
    explicit ArrayNode() = default;
    explicit ArrayNode(std::vector<Value> const &values);

    std::optional<Value> getValuesAt(size_t i) const;

    void setValue(size_t i, Value val);

    std::string toString() const;

    bool isEmpty() const { return m_values.empty(); }

    bool equalTo(ArrayNode const *other);

    virtual ~ArrayNode();

private:
    std::vector<Value> m_values;
};