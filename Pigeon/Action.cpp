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
    case Operator::Less:
    {
        break;
    }
    case Operator::More:
    {
        break;
    }
    case Operator::LessEq:
    {
        break;
    }
    case Operator::MoreEq:
    {
        break;
    }
    case Operator::Add:
    {
        break;
    }
    case Operator::Sub:
    {
        break;
    }
    case Operator::Mul:
    {
        break;
    }
    case Operator::Div:
    {
        break;
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
        break;
    }
    case Operator::And:
    {
        break;
    }
    case Operator::Or:
    {
        break;
    }
    case Operator::Not:
    {
        break;
    }
    case Operator::BitAnd:
    {
        break;
    }
    case Operator::BitOr:
    {
        break;
    }
    case Operator::BitXor:
    {
        break;
    }
    case Operator::BitNot:
    {
        break;
    }
    case Operator::BitLeftShift:
    {
        break;
    }
    case Operator::BitRightShift:
    {
        break;
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