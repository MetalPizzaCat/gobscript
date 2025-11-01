#include "StandardFunctions.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Action.hpp"
#include "../Pigeon/Error.hpp"
#include "../Pigeon/Array.hpp"

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
        for (size_t i = 0; i < i < arguments.size(); i++)
        {
            values[func.arguments[i]] = arguments[i];
        }
        state.pushVariableScope(values);
        r = func.body->execute(state);
        state.popVariableScope();
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
    throwError("IMPLEMENT !!!");
    return Value();
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
