#pragma once
#include <vector>
#include "../Pigeon/Value.hpp"
#include "../Pigeon/State.hpp"
namespace GobScriptHelper
{

    Value nativePrintLineFunction(State &state, std::vector<Value> const &args);

    Value nativeLenFunction(State &state, std::vector<Value> const &args);

    Value nativeGetFileNameSuffix(State &state, std::vector<Value> const &args);

    Value nativeArrayFilter(State &state, std::vector<Value> const &args);
}