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

static const std::map<std::string, StandardFunctionInfo> StandardFunctions = {
    {"println", StandardFunctionInfo{.argumentCount = (size_t)-1, .functionId = 0}},
    {"len", StandardFunctionInfo{.argumentCount = 1, .functionId = 1}},
    {"filter", StandardFunctionInfo{.argumentCount = 2, .functionId = 2}},
    {"map", StandardFunctionInfo{.argumentCount = 2, .functionId = 3}},
    {"listdir", StandardFunctionInfo{.argumentCount = 1, .functionId = 4}},
    {"filename_suffix", StandardFunctionInfo{.argumentCount = 1, .functionId = 5}},
    {"is_dir", StandardFunctionInfo{.argumentCount = 1, .functionId = 6}},
    {"is_file", StandardFunctionInfo{.argumentCount = 1, .functionId = 7}},
    {"append", StandardFunctionInfo{.argumentCount = 2, .functionId = 8}},
    {"at", StandardFunctionInfo{.argumentCount = 2, .functionId = 9}},
    {"filename", StandardFunctionInfo{.argumentCount = 1, .functionId = 10}},
    {"filename_stem", StandardFunctionInfo{.argumentCount = 1, .functionId = 11}},
    {"set", StandardFunctionInfo{.argumentCount = 3, .functionId = 12}},
    {"input", StandardFunctionInfo{.argumentCount = 0, .functionId = 13}},
    {"array_of_size", StandardFunctionInfo{.argumentCount = 1, .functionId = 14}},
    {"chr", StandardFunctionInfo{.argumentCount = 1, .functionId = 15}},
    {"ord", StandardFunctionInfo{.argumentCount = 1, .functionId = 16}},
    {"print", StandardFunctionInfo{.argumentCount = (size_t)-1, .functionId = 17}}};