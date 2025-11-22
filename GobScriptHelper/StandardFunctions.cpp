#include "StandardFunctions.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Action.hpp"
#include "../Pigeon/Error.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Parser.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>

State GobScriptHelper::prepareScriptState()
{
    using namespace GobScriptHelper;
    return State({nativePrintLineFunction,
                  nativeLenFunction,
                  nativeArrayFilter,
                  nativeMapArray,
                  nativeListDirectory,
                  nativeGetFileNameSuffix,
                  nativeIsDirectory,
                  nativeIsFile,
                  nativeAppend,
                  nativeAt,
                  nativeGetFileName,
                  nativeGetFileNameStem,
                  nativeSetAt,
                  nativeInput,
                  nativeCreateArrayOfSize,
                  nativeConvertCharIntToAsciiString,
                  nativeConvertCharStringToAsciiInt,
                  nativePrintFunction,
                  nativeExit});
}

std::unique_ptr<Action> GobScriptHelper::loadString(std::string const &code)
{
    std::string::const_iterator start = code.begin();
    std::vector<std::unique_ptr<Action>> acts = Pigeon::Parser::parseTopLevelDeclarations(start, code.end());

    return std::make_unique<SequenceAction>(start, std::move(acts));
}

std::unique_ptr<Action> GobScriptHelper::loadFile(std::string const &filepath)
{
    std::ifstream codeFile(filepath);

    return loadString(std::string{std::istreambuf_iterator<char>(codeFile), std::istreambuf_iterator<char>()});
}

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
            throw RuntimeActionExecutionError("Function expected " +
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

Value GobScriptHelper::nativePrintFunction(State &state, std::vector<Value> const &args)
{
    for (size_t i = 0; i < args.size(); i++)
    {
        std::cout << convertValueToString(args[0]);
        if (i != args.size() - 1)
        {
            std::cout << '\t';
        }
    }
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
        throw RuntimeActionExecutionError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).extension());
}

Value GobScriptHelper::nativeGetFileName(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throw RuntimeActionExecutionError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).filename());
}

Value GobScriptHelper::nativeGetFileNameStem(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throw RuntimeActionExecutionError("Expected string");
    }
    return state.createString(std::filesystem::path(getValueAsString(v)->getValue()).stem());
}

Value GobScriptHelper::nativeArrayFilter(State &state, std::vector<Value> const &args)
{
    Value array = args[0];
    Value callback = args[1];
    if (array.index() != ValueType::Array)
    {
        throw RuntimeActionExecutionError("Expected array");
    }
    if (callback.index() != ValueType::FunctionRef)
    {
        throw RuntimeActionExecutionError("Expected callback filter function");
    }
    ArrayNode const *arr = getValueAsArray(array);
    std::optional<ScriptFunction> func = getCallableFunction(state, getValueAsFunction(callback).id, getValueAsFunction(callback).native);
    if (!func.has_value())
    {
        throw RuntimeActionExecutionError("Referenced function not found");
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
        throw RuntimeActionExecutionError("Expected array");
    }
    if (callback.index() != ValueType::FunctionRef)
    {
        throw RuntimeActionExecutionError("Expected callback filter function");
    }
    ArrayNode const *arr = getValueAsArray(array);
    std::optional<ScriptFunction> func = getCallableFunction(state, getValueAsFunction(callback).id, getValueAsFunction(callback).native);
    if (!func.has_value())
    {
        throw RuntimeActionExecutionError("Referenced function not found");
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
        throw RuntimeActionExecutionError("Expected folder path");
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
        throw RuntimeActionExecutionError("Expected folder path");
    }
    return (int64_t)std::filesystem::is_directory(getValueAsString(path)->getValue());
}

Value GobScriptHelper::nativeIsFile(State &state, std::vector<Value> const &args)
{
    Value path = args[0];
    if (path.index() != ValueType::String)
    {
        throw RuntimeActionExecutionError("Expected folder path");
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
            throw RuntimeActionExecutionError("Expected string to append");
        }
        getValueAsString(v)->getValue() += getValueAsString(v2)->getValue();
        return v;
        break;
    case ValueType::Array:
        getValueAsArray(v)->pushBack(v2);
        return v;
        break;
    default:
        throw RuntimeActionExecutionError("Invalid argument type for append operation, expected array or string");
    }
    return Value();
}

Value GobScriptHelper::nativeAt(State &state, std::vector<Value> const &args)
{
    Value v = args[0];
    Value i = args[1];
    if (i.index() != ValueType::Integer)
    {
        throw RuntimeActionExecutionError("Expected integer for the indexing operation");
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
            throw RuntimeActionExecutionError("Array out of bounds access");
        }
        break;
    default:
        throw RuntimeActionExecutionError("Invalid argument type for indexing operation, expected array or string");
    }
    return Value();
}

Value GobScriptHelper::nativeSetAt(State &state, std::vector<Value> const &args)
{
    Value array = args[0];
    Value i = args[1];
    Value value = args[2];
    if (i.index() != ValueType::Integer)
    {
        throw RuntimeActionExecutionError("Expected integer for the indexing operation");
    }
    switch (array.index())
    {
    case ValueType::String:
        switch (value.index())
        {
        case ValueType::Integer:
            getValueAsString(array)->getValue()[getValueAsInt(i)] = getValueAsInt(value);
            return value;
        case ValueType::String:
            if (getValueAsString(value)->getValue().empty())
            {
                throw RuntimeActionExecutionError("To assign character to string using string value, string must be at least 1 character long. Rest of the string is ignored");
            }
            getValueAsString(array)->getValue()[getValueAsInt(i)] = getValueAsString(value)->getValue()[0];
            return value;
        default:
            throw RuntimeActionExecutionError("Expected integer or string for 'set at' operator");
        }
        break;
    case ValueType::Array:
        getValueAsArray(array)->setValue(getValueAsInt(i), value);
        return value;
        break;
    default:
        throw RuntimeActionExecutionError("Invalid argument type for indexing operation, expected array or string");
    }
    return Value();
}

Value GobScriptHelper::nativeInput(State &state, std::vector<Value> const &args)
{
    std::string input;

    std::cin >> input;
    return state.createString(input);
}

Value GobScriptHelper::nativeCreateArrayOfSize(State &state, std::vector<Value> const &args)
{
    Value size = args[0];
    if (size.index() != ValueType::Integer)
    {
        throw RuntimeActionExecutionError("Expected integer for array size");
    }
    return state.createArray(std::vector<Value>(getValueAsInt(size), Value(0)));
}

Value GobScriptHelper::nativeConvertCharIntToAsciiString(State &state, std::vector<Value> const &args)
{
    Value ch = args[0];
    if (ch.index() != ValueType::Integer)
    {
        throw RuntimeActionExecutionError("Expected integer");
    }
    return state.createString(std::string{(char)getValueAsInt(ch)});
}

Value GobScriptHelper::nativeConvertCharStringToAsciiInt(State &state, std::vector<Value> const &args)
{
    Value str = args[0];
    if (str.index() != ValueType::String)
    {
        throw RuntimeActionExecutionError("Expected string");
    }
    if (getValueAsString(str)->getLen() != 1)
    {
        throw RuntimeActionExecutionError(std::string("Expected single character, but found string of length ") + std::to_string(getValueAsString(str)->getLen()));
    }
    return (IntegerType)getValueAsString(str)->getValue()[0];
}

Value GobScriptHelper::nativeExit(State &state, std::vector<Value> const &args)
{
    Value str = args[0];
    if (str.index() != ValueType::Integer)
    {
        throw RuntimeActionExecutionError("Expected exit code");
    }
    exit(getValueAsInt(str));
}
