#pragma once

#include <string>
#include <map>

/// @brief Structure describing basic info for the native "standard library" functions
struct StandardFunctionInfo
{
    /// @brief Amount of arguments this function should have or -1 for no restrictions
    size_t argumentCount;
    /// @brief Which function in this state does this function refer to. Used to avoid broken ids on change
    size_t functionId;
};
static const std::map<std::string, StandardFunctionInfo> StandardFunctionIds = {
    {"print", StandardFunctionInfo{.argumentCount = (size_t)-1, .functionId = 0}},
    {"len", StandardFunctionInfo{.argumentCount = 1, .functionId = 1}},
    {"file_suffix", StandardFunctionInfo{.argumentCount = 1, .functionId = 2}}
};