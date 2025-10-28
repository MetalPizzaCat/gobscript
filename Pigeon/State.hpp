#pragma once
#include "Memory.hpp"
#include "Value.hpp"
#include <map>
#include <vector>
#include <optional>
class State
{
public:
    StringNode *createString(std::string const &base)
    {
        StringNode *node = new StringNode(base);
        m_root.pushBack(node);
        return node;
    }

    // std::vector<Value> *createArray()
    // {
    //     m_arrays.push_back(std::make_unique<std::vector<Value>>({}));
    //     return m_arrays.back().get();
    // }
    
    std::optional<Value> getVariableValue(std::string const& name);

    std::optional<Value> setVariableValue(std::string const& name, Value val);

    bool doesVariableExist(std::string const& name);

    bool doesVariableExistAndOfType(std::string const& name, ValueType type);

    /// @brief Create a new set of variables that will be available in a given block
    /// @param variables List of all variables to create with default values
    void pushVariableScope(std::map<std::string, Value> variables);

    /// @brief Pop past variable scope and free up memory of all unused objects
    void popVariableScope();

private:
    std::vector<std::map<std::string, Value>> m_variables;
    MemoryNode m_root;
};