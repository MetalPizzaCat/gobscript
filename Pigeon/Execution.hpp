#pragma once

#include <memory>
#include "Value.hpp"
#include "Action.hpp"

/// @brief Execute function putting  the arguments into the state variable storage and applying reference counting procedures
/// @param state Current state in which this execution happens
/// @param action Action representing the function body
/// @param arguments Dictionary of of argument_name-value pairs representing the values of arguments
/// @return Value returned by the action body
Value executeFunction(State &state, std::unique_ptr<Action> action, std::map<std::string, Value> const& arguments);