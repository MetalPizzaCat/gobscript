#pragma once
#include "Memory.hpp"
#include "Value.hpp"
#include "Array.hpp"
#include <map>
#include <vector>
#include <optional>
#include <functional>
#include "Function.hpp"
class State
{
public:
    using NativeFunction = std::function<Value(std::vector<Value> const &args)>;

    explicit State() = default;

    explicit State(std::vector<NativeFunction> const &funcs) : m_standardFunctions(funcs) {}

    /// @brief  Create a new string object and store it in the state memory
    /// @param base Inital value for the string object
    /// @return Pointer to the string object
    StringNode *createString(std::string const &base);

    /// @brief Create a new array object and store it in the state memory
    /// @param values Inital contents of the array
    /// @return Pointer to the array object
    ArrayNode *createArray(std::vector<Value> const values);

    /// @brief Attempt to get the value of a variable in the state
    /// @param name Name of the variable
    /// @return Value of the variable or None if no variable exists
    std::optional<Value> getVariableValue(std::string const &name);

    /// @brief Attempt to set value of a variable in the state
    /// @param name Name of the variable
    /// @param val Value of the variable
    /// @return New value or none if variable doesn't exist
    std::optional<Value> setVariableValue(std::string const &name, Value val);

    bool doesVariableExist(std::string const &name);

    bool doesVariableExistAndOfType(std::string const &name, ValueType type);

    /// @brief Create a new set of variables that will be available in a given block
    /// @param variables List of all variables to create with default values
    void pushVariableScope(std::map<std::string, Value> variables);

    /// @brief Pop past variable scope and free up memory of all unused objects
    void popVariableScope();

    /// @brief Register user function in the state under a given name
    /// @param name Name of the function used for calling
    /// @param arguments Names of the arguments which will be used for creating variables
    /// @param body Pointer to the action representing function body
    void addFunction(std::string const &name, std::vector<std::string> arguments, Action const *body);

    /// @brief Try to find user function data by name
    /// @param name Name of the function to find
    /// @return Structure storing function data or None if no function uses that name
    std::optional<Function> getFunction(std::string const &name) const;

    std::optional<NativeFunction> getStandardFunction(size_t i) const;
    void collectGarbage();

private:
    std::vector<NativeFunction> m_standardFunctions;
    std::vector<std::map<std::string, Value>> m_variables;
    MemoryNode m_root;
    std::map<std::string, Function> m_functions;
};