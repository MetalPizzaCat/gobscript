#pragma once
#include <vector>
#include "../Pigeon/Value.hpp"
#include "../Pigeon/State.hpp"
namespace GobScriptHelper
{
    /// @brief Abstraction around both Pigeon and c++ functions to help with callbacks
    using ScriptFunction = std::variant<Function, State::NativeFunction>;

    /// @brief Attempt to retrieve a function with a given id
    /// @param state State to search the function in
    /// @param id Id of the function
    /// @param native Whether to look in Pigeon function list or c++ function list
    /// @return Union containing one of the value types or None if no function with given id is found
    std::optional<ScriptFunction> getCallableFunction(State &state, size_t id, bool native);

    /// @brief Call script function by either executing the expression tree or calling c++ function depending on script function type
    /// @param state State of the system to call in
    /// @param f Function to call
    /// @param arguments Arguments to pass into the function 
    /// @return Value returned by the function `f`
    Value callScriptFunction(State &state, ScriptFunction const &f, std::vector<Value> const &arguments);

    Value nativePrintLineFunction(State &state, std::vector<Value> const &args);

    Value nativeLenFunction(State &state, std::vector<Value> const &args);

    Value nativeGetFileNameSuffix(State &state, std::vector<Value> const &args);

    Value nativeArrayFilter(State &state, std::vector<Value> const &args);

    Value nativeMapArray(State &state, std::vector<Value> const &args);

    Value nativeTestDouble(State &state, std::vector<Value> const &args);
}