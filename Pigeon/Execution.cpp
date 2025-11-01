#include "Execution.hpp"

Value executeFunction(State &state, std::unique_ptr<Action> action, std::map<std::string, Value> const &arguments)
{
    state.pushVariableScope(arguments);
    Value result = action->execute(state);
    state.popVariableScope();
    return result;
}