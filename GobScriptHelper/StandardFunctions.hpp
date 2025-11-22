#pragma once
#include <vector>
#include "../Pigeon/Value.hpp"
#include "../Pigeon/State.hpp"
namespace GobScriptHelper
{
    /// @brief Create a script state object that can be used for gsh
    /// @return
    State prepareScriptState();

    /// @brief Abstraction around both Pigeon and c++ functions to help with callbacks
    using ScriptFunction = std::variant<Function, State::NativeFunction>;

    /// @brief Attempt to parse given code string and return a runnable action object
    /// @param code Code to parse
    /// @return Smart pointer containing runnable action or nullptr if parsing failed. Exception is thrown on parsing error
    std::unique_ptr<Action> loadString(std::string const &code);

    std::unique_ptr<Action> loadFile(std::string const &filepath);

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

    Value nativePrintFunction(State &state, std::vector<Value> const &args);

    Value nativeLenFunction(State &state, std::vector<Value> const &args);

    Value nativeGetFileNameSuffix(State &state, std::vector<Value> const &args);

    Value nativeGetFileName(State &state, std::vector<Value> const &args);

    Value nativeGetFileNameStem(State &state, std::vector<Value> const &args);

    Value nativeArrayFilter(State &state, std::vector<Value> const &args);

    Value nativeMapArray(State &state, std::vector<Value> const &args);

    Value nativeTestDouble(State &state, std::vector<Value> const &args);

    Value nativeListDirectory(State &state, std::vector<Value> const &args);

    Value nativeIsDirectory(State &state, std::vector<Value> const &args);

    Value nativeIsFile(State &state, std::vector<Value> const &args);

    Value nativeAppend(State &state, std::vector<Value> const &args);

    Value nativeAt(State &state, std::vector<Value> const &args);

    Value nativeSetAt(State &state, std::vector<Value> const &args);

    Value nativeInput(State &state, std::vector<Value> const &args);

    Value nativeCreateArrayOfSize(State &state, std::vector<Value> const &args);

    /// @brief Convert an integer value into a 1 character long string with ASCII character based on given value
    /// @param state 
    /// @param args 
    /// @return 
    Value nativeConvertCharIntToAsciiString(State &state, std::vector<Value> const &args);

    /// @brief Convert single character string into an integer value equal to the ASCII code of the character
    /// @param state 
    /// @param args 
    /// @return 
    Value nativeConvertCharStringToAsciiInt(State &state, std::vector<Value> const &args);

    /// @brief Ends execution of the program returning the first argument converted to int as the exit code. This function does not return anything and ends entire program execution
    /// @param state 
    /// @param args 
    /// @return 
    Value nativeExit(State &state, std::vector<Value> const &args);
}