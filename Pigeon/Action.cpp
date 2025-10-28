#include "Action.hpp"
#include "State.hpp"
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
    case Operator::Assign:
    {
        break;
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
    case Operator::AddAssign:
    {
        break;
    }
    case Operator::SubAssign:
    {
        break;
    }
    case Operator::MulAssign:
    {
        break;
    }
    case Operator::DivAssign:
    {
        break;
    }
    case Operator::ModuloAssign:
    {
        break;
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
    case Operator::BitAndAssign:
    {
        break;
    }
    case Operator::BitOrAssign:
    {
        break;
    }
    case Operator::BitXorAssign:
    {
        break;
    }
    case Operator::BitNotAssign:
    {
        break;
    }
    case Operator::BitLeftShiftAssign:
    {
        break;
    }
    case Operator::BitRightShiftAssign:
    {
        break;
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
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);
        execv(programName.c_str(), (char *const *)args.data());
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