#include "Function.hpp"
#include <string>
#include "Action.hpp"
Function::Function(Action const* body, std::vector<std::string> const&arguments) : body(body), arguments(arguments) {}