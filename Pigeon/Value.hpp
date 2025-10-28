#pragma once

#include <variant>
#include <string>
#include <stdexcept>
#include "Error.hpp"
#include "Memory.hpp"

using Value = std::variant<int64_t, StringNode *, ArrayNode *>;

enum ValueType
{
    Integer = 0,
    String = 1,
    Array = 2
};

std::string convertValueToString(Value const &val);

bool isValueNull(Value const &val);

/**
 * @brief  Check if two values can be considered same. This is similar to non strict comparison in javascript, except different
 *
 * Specifically [], "" and 0 are equal to each other and can be used as a "null" value.
 *
 * - Integer to string comparison -> Integer gets converted to string and compared to string. Primary use of this operator
 *
 * - Integer to array comparison -> Returns false in each case unless both values are "null"
 *
 * - String to array comparison -> Returns false in each case unless both values are "null"
 *
 * - String to string comparison -> Returns true if both strings are exact
 *
 * - Array to array comparison -> Returns true if both arrays have same contents
 *
 * @param a
 * @param b
 * @return true
 * @return false
 */
bool areValuesTheSame(Value const &a, Value const &b);

/// @brief Check if values are equal. This means they have to match both on type and value. For integer it means same value, but for array and string it means same pointer
/// @param a
/// @param b
/// @return
bool areValuesEqual(Value const &a, Value const &b);