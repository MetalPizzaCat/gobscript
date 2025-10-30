#include "Action.hpp"
#include "State.hpp"
#include "Function.hpp"
Value BinaryOperationAction::execute(State &state) const
{
    Value a = getArgument(0)->execute(state);
    Value b = getArgument(1)->execute(state);
    switch (m_op)
    {
    case Operator::Equals:
    {
        return areValuesTheSame(a, b);
    }
    case Operator::NotEquals:
    {
        return !areValuesTheSame(a, b);
    }
    case Operator::EqualsStrict:
    {
        return areValuesEqual(a, b);
    }
    case Operator::NotEqualsStrict:
    {
        return !areValuesEqual(a, b);
    }
    }

    if (a.index() != b.index() || a.index() != ValueType::Integer)
    {
        throwError("Expected both values to be integers");
    }
    switch (m_op)
    {

    case Operator::Less:
    {
        return Value(std::get<int64_t>(a) < std::get<int64_t>(b));
    }
    case Operator::More:
    {
        return Value(std::get<int64_t>(a) > std::get<int64_t>(b));
    }
    case Operator::LessEq:
    {
        return Value(std::get<int64_t>(a) <= std::get<int64_t>(b));
    }
    case Operator::MoreEq:
    {
        return Value(std::get<int64_t>(a) >= std::get<int64_t>(b));
    }
    case Operator::Add:
    {
        return Value(std::get<int64_t>(a) + std::get<int64_t>(b));
    }
    case Operator::Sub:
    {
        return Value(std::get<int64_t>(a) - std::get<int64_t>(b));
    }
    case Operator::Mul:
    {
        return Value(std::get<int64_t>(a) * std::get<int64_t>(b));
    }
    case Operator::Div:
    {
        return Value(std::get<int64_t>(a) / std::get<int64_t>(b));
    }

    case Operator::Modulo:
    {
        return Value(std::get<int64_t>(a) % std::get<int64_t>(b));
    }
    case Operator::And:
    {
        return Value(std::get<int64_t>(a) && std::get<int64_t>(b));
    }
    case Operator::Or:
    {
        return Value(std::get<int64_t>(a) || std::get<int64_t>(b));
    }
    case Operator::Not:
    {
        break;
    }
    case Operator::BitAnd:
    {
        return Value(std::get<int64_t>(a) & std::get<int64_t>(b));
    }
    case Operator::BitOr:
    {
        return Value(std::get<int64_t>(a) | std::get<int64_t>(b));
    }
    case Operator::BitXor:
    {
        return Value(std::get<int64_t>(a) ^ std::get<int64_t>(b));
    }
    case Operator::BitNot:
    {
        break;
    }
    case Operator::BitLeftShift:
    {
        return Value(std::get<int64_t>(a) << std::get<int64_t>(b));
    }
    case Operator::BitRightShift:
    {
        return Value(std::get<int64_t>(a) >> std::get<int64_t>(b));
    }
    }
    return a;
}

Value CommandCallAction::execute(State &state) const
{
    std::string programName = convertValueToString(m_commandName->execute(state));
    std::vector<std::string> argsV;
    for (std::unique_ptr<Action> const &arg : m_arguments)
    {
        argsV.push_back(convertValueToString(arg->execute(state)));
    }
    std::vector<const char *> args = {programName.c_str()};
    for (std::string const &arg : argsV)
    {
        args.push_back(arg.c_str());
    }
    args.push_back(nullptr);
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);
        execvp(programName.c_str(), (char *const *)args.data());
        exit(127);
    }
    else
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        close(pipefd[1]);
        while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
        {
            std::cout << buffer << std::endl;
        }
        int status;
        waitpid(pid, &status, 0);
        return Value(status);
    }
}

Value VariableBlockAction::execute(State &state) const
{
    std::map<std::string, Value> variables;
    for (std::pair<const std::string, std::unique_ptr<Action>> const &var : m_variables)
    {
        variables[var.first] = var.second->execute(state);
    }
    state.pushVariableScope(variables);
    Value result = m_body->execute(state);
    state.popVariableScope();
    return result;
}

Value VariableAccessAction::execute(State &state) const
{
    if (std::optional<Value> val = state.getVariableValue(m_name); val.has_value())
    {
        return val.value();
    }
    throwError("No variable with name " + m_name + " exists");
    return Value();
}

Value AssignOperationAction::execute(State &state) const
{

    Value val = m_value->execute(state);
    if (m_op == Operator::Assign)
    {
        state.setVariableValue(m_name, val);
        return val;
    }
    if (!state.doesVariableExistAndOfType(m_name, ValueType::Integer))
    {
        return Value(0);
    }
    switch (m_op)
    {
    case Operator::AddAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) + std::get<int64_t>(val)).value();
    }
    case Operator::SubAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) - std::get<int64_t>(val)).value();
    }
    case Operator::MulAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) * std::get<int64_t>(val)).value();
    }
    case Operator::DivAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) / std::get<int64_t>(val)).value();
    }
    case Operator::ModuloAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) % std::get<int64_t>(val)).value();
    }
    case Operator::BitAndAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) & std::get<int64_t>(val)).value();
    }
    case Operator::BitOrAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) | std::get<int64_t>(val)).value();
    }
    case Operator::BitXorAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) ^ std::get<int64_t>(val)).value();
    }
    case Operator::BitNotAssign:
    {
        return Value(0);
    }
    case Operator::BitLeftShiftAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) << std::get<int64_t>(val)).value();
    }
    case Operator::BitRightShiftAssign:
    {
        return state.setVariableValue(m_name, std::get<int64_t>(state.getVariableValue(m_name).value()) >> std::get<int64_t>(val)).value();
    }
    }
    return Value();
}

Value CreateArrayAction::execute(State &state) const
{
    std::vector<Value> values;
    for (auto const &action : getArguments())
    {
        values.push_back(action->execute(state));
    }
    return Value(state.createArray(values));
}

Value BranchAction::execute(State &state) const
{
    Value cond = m_cond->execute(state);
    if (cond.index() != ValueType::Integer)
    {
        throwError("Expected an integer");
    }
    if (std::get<int64_t>(cond))
    {
        return m_then->execute(state);
    }
    else if (m_else != nullptr)
    {
        return m_else->execute(state);
    }
    return Value(0);
}

Value SequenceAction::execute(State &state) const
{
    Value result = Value(0);
    for (size_t i = 0; i < getArgumentCount(); i++)
    {
        auto it = getArgument(i);
        if (getArgument(i) != nullptr)
        {
            result = getArgument(i)->execute(state);
        }
    }
    return result;
}

Value FunctionDeclarationAction::execute(State &state) const
{
    state.addFunction(m_name, m_arguments, m_body.get());
    return Value();
}

Value FunctionCallAction::execute(State &state) const
{
    std::optional<Function> f = state.getFunction(m_name);
    if (!f.has_value())
    {
        throwError("No function named '" + m_name + "' found");
    }
    if (f.value().arguments.size() != getArgumentCount())
    {
        throwError("Function '" + m_name + "' expected " + std::to_string(f.value().arguments.size()) + " arguments, but got " + std::to_string(getArgumentCount()));
    }
    std::map<std::string, Value> variables;
    for (size_t i = 0; i < getArgumentCount(); i++)
    {
        variables[f.value().arguments[i]] = getArgument(i)->execute(state);
    }
    state.pushVariableScope(variables);
    Value result = f.value().body->execute(state);
    state.popVariableScope();
    return result;
}
