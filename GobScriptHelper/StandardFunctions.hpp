#pragma once
#include <vector>
#include "../Pigeon/Value.hpp"

namespace GobScriptHelper
{

    Value nativePrintLineFunction(std::vector<Value> const &args);

    Value nativeLenFunction(std::vector<Value> const &args);

    Value nativeGetFileNameSuffix(std::vector<Value> const &args);
}