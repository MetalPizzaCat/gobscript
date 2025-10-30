#pragma once
#include "Memory.hpp"
#include "Value.hpp"
#include "Array.hpp"
#include <map>
#include <vector>
#include <optional>
#include "Function.hpp"
class State
{
public:
    StringNode *createString(std::string const &base);

    ArrayNode *createArray(std::vector<Value> const values);

    std::optional<Value> getVariableValue(std::string const &name);

    std::optional<Value> setVariableValue(std::string const &name, Value val);

    bool doesVariableExist(std::string const &name);

    bool doesVariableExistAndOfType(std::string const &name, ValueType type);

    /// @brief Create a new set of variables that will be available in a given block
    /// @param variables List of all variables to create with default values
    void pushVariableScope(std::map<std::string, Value> variables);

    /// @brief Pop past variable scope and free up memory of all unused objects
    void popVariableScope();

    void addFunction(std::string const &name, std::vector<std::string> arguments, Action const *body);

    std::optional<Function> getFunction(std::string const& name) const;
private:
    std::vector<std::map<std::string, Value>> m_variables;
    MemoryNode m_root;
    std::map<std::string, Function> m_functions;
};