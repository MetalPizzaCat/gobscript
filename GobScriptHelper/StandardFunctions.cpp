#include "StandardFunctions.hpp"
#include "../Pigeon/Array.hpp"
#include "../Pigeon/Error.hpp"
Value GobScriptHelper::nativePrintLineFunction(std::vector<Value> const &args)
{
    for (Value const &v : args)
    {
        std::cout << convertValueToString(v) << '\t';
    }
    std::cout << std::endl;
    return Value(0);
}

Value GobScriptHelper::nativeLenFunction(std::vector<Value> const &args)
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

Value GobScriptHelper::nativeGetFileNameSuffix(std::vector<Value> const &args)
{
    Value v = args[0];
    if (v.index() != ValueType::String)
    {
        throwError("Expected string");
    }
    throwError("IMPLEMENT !!!");
    return Value();
}
