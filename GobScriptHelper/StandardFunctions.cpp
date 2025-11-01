#include "StandardFunctions.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Action.hpp"
#include "../Pigeon/Error.hpp"
#include "../Pigeon/Array.hpp"

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
    std::optional<Function> f = state.getUserFunctionById(getValueAsFunction(callback).id);
    if (!f.has_value())
    {
        throwError("Referenced function not found");
    }
    if (f.value().arguments.size() != 1)
    {
        throwError("Function '" + state.getUserFunctionNameById(getValueAsFunction(callback).id).value() + "' expected " + std::to_string(f.value().arguments.size()) + " arguments, but got 1");
    }

    std::vector<Value> val;
    for (size_t i = 0; i < arr->getLen(); i++)
    {
        std::map<std::string, Value> variables = {{f.value().arguments[0], arr->getValueAt(i).value()}};

        state.pushVariableScope(variables);
        Value result = f.value().body->execute(state);
        if (getValueAsInt(result))
        {
            val.push_back(arr->getValueAt(i).value());
        }
        state.popVariableScope();
    }
    return state.createArray(val);
}
