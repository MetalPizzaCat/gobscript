#include "Action.hpp"
#include "State.hpp"
Value BinaryOperationAction::execute(State &state) const
{
    Value a = getArgument(0)->execute(state);
    Value b = getArgument(1)->execute(state);

    return a;
}