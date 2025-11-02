#include "StandardFunctions.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Action.hpp"
#include "../Pigeon/Error.hpp"
#include "../Pigeon/Array.hpp"
#include <filesystem>

std::optional<GobScriptHelper::ScriptFunction> GobScriptHelper::getCallableFunction(State &state, size_t id, bool native)
{
    if (!native)
    {
        return state.getUserFunctionById(id);
    }
    else
    {
        return state.getStandardFunction(id);
    }
}

Value GobScriptHelper::callScriptFunction(State &state, ScriptFunction const &f, std::vector<Value> const &arguments)
{
    Value r;
    if (f.index() == 0)
    {
        Function func = std::get<Function>(f);
        if (arguments.size() != func.arguments.size())
        {
            throwError("Function expected " +
                       std::to_string(func.arguments.size()) +
                       " arguments, but got 1");
        }
        std::map<std::string, Value> values;
        for (size_t i = 0; i < arguments.size(); i++)
        {
            values[func.arguments[i]] = arguments[i];
            increaseValueRefCount(values[func.arguments[i]]);
        }
        state.pushVariableScope(values);
        r = func.body->execute(state);
        state.popVariableScope();

        for (auto const &v : values)
        {
            decreaseValueRefCount(v.second);
        }
    }
    else
    {
        for (auto const &arg : arguments)
        {
            increaseValueRefCount(arg);
        }
        r = std::get<State::NativeFunction>(f)(state, arguments);
        for (auto const &arg : arguments)
        {
            decreaseValueRefCount(arg);
        }
    }
    return r;
}

Value GobScriptHelper::nativePrintLineFunction(State &state, std::vector<Value> const &args)
{
    for (Value const &v : args)
    {
        std::cout << convertValueToString(v) << '\t';
    }
    std::cout << std::endl;
    return Value(0);
}

Value GobScriptHelper::nativeLenFunction(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    switch (v.index())
    {
    case ValueType::Integer:
        return Value(1);
    case ValueType::String:
        return Value((int64_t)std::get<StringNode *>(v)->getLen());
    case ValueType::Array:
        return Value((int64_t)std::get<ArrayNode *>(v)->getLen());
    }
    return Value();
}

Value GobScriptHelper::nativeGetFileNameSuffix(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throwError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).extension());
}

Value GobScriptHelper::nativeGetFileName(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throwError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).filename());
}

Value GobScriptHelper::nativeGetFileNameStem(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throwError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).stem());
}

Value GobScriptHelper::nativeArrayFilter(State &state, std::vector<Value> const &args)
{
    Value array = args[0];
    Value callback = args[1];
    if (array.index() != ValueType::Array)
    {
        throwError("Expected array");
    }
    if (callback.index() != ValueType::FunctionRef)
    {
        throwError("Expected callback filter function");
    }
    ArrayNode const *arr = getValueAsArray(array);
    std::optional<ScriptFunction> func = getCallableFunction(state, getValueAsFunction(callback).id, getValueAsFunction(callback).native);
    if (!func.has_value())
    {
        throwError("Referenced function not found");
    }
    std::vector<Value> val;
    for (size_t i = 0; i < arr->getLen(); i++)
    {
        Value result = callScriptFunction(state, func.value(), {arr->getValueAt(i).value()});
        if (getValueAsInt(result))
        {
            val.push_back(arr->getValueAt(i).value());
        }
    }
    return state.createArray(val);
}

Value GobScriptHelper::nativeMapArray(State &state, std::vector<Value> const &args)
{
    Value array = args[0];
    Value callback = args[1];
    if (array.index() != ValueType::Array)
    {
        throwError("Expected array");
    }
    if (callback.index() != ValueType::FunctionRef)
    {
        throwError("Expected callback filter function");
    }
    ArrayNode const *arr = getValueAsArray(array);
    std::optional<ScriptFunction> func = getCallableFunction(state, getValueAsFunction(callback).id, getValueAsFunction(callback).native);
    if (!func.has_value())
    {
        throwError("Referenced function not found");
    }

    std::vector<Value> val;
    for (size_t i = 0; i < arr->getLen(); i++)
    {
        val.push_back(callScriptFunction(state, func.value(), {arr->getValueAt(i).value()}));
    }
    return state.createArray(val);
}

Value GobScriptHelper::nativeTestDouble(State &state, std::vector<Value> const &args)
{
    return Value(getValueAsInt(args[0]) * 2);
}

Value GobScriptHelper::nativeListDirectory(State &state, std::vector<Value> const &args)
{
    Value path = args[0];
    if (path.index() != ValueType::String)
    {
        throwError("Expected folder path");
    }
    if (!std::filesystem::is_directory(getValueAsString(path)->getValue()))
    {
        return Value(0);
    }
    std::vector<Value> files;
    for (const auto &entry : std::filesystem::directory_iterator(getValueAsString(path)->getValue()))
    {
        files.push_back(state.createString(entry.path()));
    }
    return state.createArray(files);
}

Value GobScriptHelper::nativeIsDirectory(State &state, std::vector<Value> const &args)
{
    Value path = args[0];
    if (path.index() != ValueType::String)
    {
        throwError("Expected folder path");
    }
    return (int64_t)std::filesystem::is_directory(getValueAsString(path)->getValue());
}

Value GobScriptHelper::nativeIsFile(State &state, std::vector<Value> const &args)
{
    Value path = args[0];
    if (path.index() != ValueType::String)
    {
        throwError("Expected folder path");
    }
    return (int64_t)std::filesystem::is_regular_file(getValueAsString(path)->getValue());
}

Value GobScriptHelper::nativeAppend(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    Value v2 = args[1];
    switch (v.index())
    {
    case ValueType::String:
        if (v2.index() != ValueType::String)
        {
            throwError("Expected string to append");
        }
        getValueAsString(v)->getValue() += getValueAsString(v2)->getValue();
        return v;
        break;
    case ValueType::Array:
        getValueAsArray(v)->pushBack(v2);
        return v;
        break;
    default:
        throwError("Invalid argument type for append operation, expected array or string");
    }
    return Value();
}

Value GobScriptHelper::nativeAt(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    Value i = args[1];
    if (i.index() != ValueType::Integer)
    {
        throwError("Expected integer for the indexing operation");
    }
    switch (v.index())
    {
    case ValueType::String:

        return state.createString(std::string{getValueAsString(v)->getValue()[getValueAsInt(i)]});
        break;
    case ValueType::Array:
        if (std::optional<Value> item = getValueAsArray(v)->getValueAt(getValueAsInt(i)); item.has_value())
        {
            return item.value();
        }
        else
        {
            throwError("Array out of bounds access");
        }
        break;
    default:
        throwError("Invalid argument type for indexing operation, expected array or string");
    }
    return Value();
}
