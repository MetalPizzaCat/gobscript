#include "Action.hpp"
#include "State.hpp"
#include "Function.hpp"

#include "StandardFunctions.hpp"

#if (defined(LINUX) || defined(__linux__) || defined(__CYGWIN__))
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */
#elif (defined(_WIN32) || defined(_WIN64))
#include <Windows.h>
#include <locale>
#include <codecvt>
// Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString(DWORD errorMessageID)
{
    if (errorMessageID == 0)
    {
        return std::string(); // No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}
#endif

Value callNativeFunction(State &state, size_t funcId, std::vector<std::unique_ptr<Action>> const &arguments)
{
    if (std::optional<State::NativeFunction> f = state.getStandardFunction(funcId); f.has_value())
    {
        std::vector<Value> argValues;
        for (size_t i = 0; i < arguments.size(); i++)
        {
            Value var = arguments.at(i)->execute(state);
            // this is made to align with how local functions are called and
            // to prevent garbage collector from destroying objects during internal function calls withing c++ function
            increaseValueRefCount(var);
            argValues.push_back(var);
        }

        Value res = f.value()(state, argValues);

        for (Value &var : argValues)
        {
            decreaseValueRefCount(var);
        }
        return res;
    }
    throw RuntimeActionExecutionError("Invalid standard library function referenced");
    return Value();
}
Value BinaryOperationAction::execute(State &state) const
{
    switch (m_op)
    {
    case Operator::And:
    {
        return Value((int64_t)(std::get<int64_t>(getArgument(0)->execute(state)) && std::get<int64_t>(getArgument(1)->execute(state))));
    }
    case Operator::Or:
    {
        return Value((int64_t)(std::get<int64_t>(getArgument(0)->execute(state)) || std::get<int64_t>(getArgument(1)->execute(state))));
    }
    }
    Value a = getArgument(0)->execute(state);
    Value b = getArgument(1)->execute(state);
    switch (m_op)
    {
    case Operator::Equals:
    {
        return (int64_t)areValuesTheSame(a, b);
    }
    case Operator::NotEquals:
    {
        return (int64_t)(!areValuesTheSame(a, b));
    }
    case Operator::EqualsStrict:
    {
        return (int64_t)areValuesEqual(a, b);
    }
    case Operator::NotEqualsStrict:
    {
        return (int64_t)!(areValuesEqual(a, b));
    }
    }

    if (a.index() == ValueType::String && b.index() == ValueType::String && m_op == Operator::Add)
    {
        return state.createString(getValueAsString(a)->getValue() + getValueAsString(b)->getValue());
    }
    if (a.index() != b.index() || a.index() != ValueType::Integer)
    {
        throwRuntimeError(getCodePosition(), "Expected both values to be integers");
    }
    switch (m_op)
    {

    case Operator::Less:
    {
        return Value((int64_t)(std::get<int64_t>(a) < std::get<int64_t>(b)));
    }
    case Operator::More:
    {
        return Value((int64_t)(std::get<int64_t>(a) > std::get<int64_t>(b)));
    }
    case Operator::LessEq:
    {
        return Value((int64_t)(std::get<int64_t>(a) <= std::get<int64_t>(b)));
    }
    case Operator::MoreEq:
    {
        return Value((int64_t)(std::get<int64_t>(a) >= std::get<int64_t>(b)));
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

#if (defined(LINUX) || defined(__linux__) || defined(__CYGWIN__))
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
            memset(buffer, 0, sizeof(buffer));
        }
        int status;
        waitpid(pid, &status, 0);
        return Value(status);
    }
#elif (defined(_WIN32) || defined(_WIN64))


    std::string cmd = programName + " ";
    for (std::unique_ptr<Action> const &arg : m_arguments)
    {
        cmd += convertValueToString(arg->execute(state)) + " ";
    }
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    // windows requires `char*` instead of const char* for some reason
    // so we create temp buffer  
    char* cmdStr = new char[cmd.size() + 1];
    memcpy(cmdStr, cmd.c_str(), cmd.size());
    cmdStr[cmd.size()] = 0;
    // create process using win32 api
    // this does create function similar to the one in unix
    // but while i have been able to check that output works
    // the input i haven't been able to test

    // note: i have practically 0 experience with WIN32 API so if someone is reading it and knows how to
    // achieve better result, you are more then welcome to help!
    // TODO: Improve subprocess handling
    if (!CreateProcess(nullptr,         // we don't need any modules
                       cmdStr,          // command we want to run
                       nullptr,
                       nullptr,
                       true,            // we make it inherit handles, to mimic piping(i think?)
                       0,
                       nullptr,
                       nullptr,
                       &si,
                       &pi))
    {
        delete cmdStr;
        DWORD err = GetLastError();
        std::cerr << "Failed to start process " << GetLastErrorAsString(err) << std::endl;
        return err;
    }
    delete cmdStr;
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
#else
#error "Unknown platform detected, please implement `exec` action process calling"
#endif
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
    // we need to preserve value outside of the block it was created in. generally it doesn't matter but for pointer types gc could be an issue
    increaseValueRefCount(result);
    state.popVariableScope();
    decreaseValueRefCount(result);
    return result;
}

Value VariableAccessAction::execute(State &state) const
{
    if (std::optional<Value> val = state.getVariableValue(m_name); val.has_value())
    {
        return val.value();
    }
    throwRuntimeError(getCodePosition(), "No variable with name " + m_name + " exists");
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
        throwRuntimeError(getCodePosition(), "Expected an integer");
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
    return Value(FunctionReference{.id = (uint32_t)state.getUserFunctionIdByName(m_name).value(), .native = false});
}

Value FunctionCallAction::execute(State &state) const
{
    Value funcId = m_functionAccess->execute(state);
    if (funcId.index() != ValueType::FunctionRef)
    {
        throwRuntimeError(getCodePosition(), "Expected function reference");
    }
    if (getValueAsFunction(funcId).native)
    {
        return callNativeFunction(state, getValueAsFunction(funcId).id, getArguments());
    }
    std::optional<Function> f = state.getUserFunctionById(getValueAsFunction(funcId).id);
    if (!f.has_value())
    {
        throwRuntimeError(getCodePosition(), "Referenced function not found");
    }
    if (f.value().arguments.size() != getArgumentCount())
    {
        throwRuntimeError(getCodePosition(), "Function '" + state.getUserFunctionNameById(getValueAsFunction(funcId).id).value() + "' expected " + std::to_string(f.value().arguments.size()) + " arguments, but got " + std::to_string(getArgumentCount()));
    }
    std::map<std::string, Value> variables;
    for (size_t i = 0; i < getArgumentCount(); i++)
    {
        variables[f.value().arguments[i]] = getArgument(i)->execute(state);
        increaseValueRefCount(variables[f.value().arguments[i]]);
    }
    state.pushVariableScope(variables);
    Value result = f.value().body->execute(state);
    increaseValueRefCount(result);
    state.popVariableScope();
    decreaseValueRefCount(result);
    for (auto const &v : variables)
    {
        decreaseValueRefCount(v.second);
    }
    return result;
}

Value ForLoopAction::execute(State &state) const
{
    Value result = Value(0);
    for (m_init->execute(state); !isValueNull(m_cond->execute(state)); m_iter->execute(state))
    {
        result = m_body->execute(state);
    }
    return result;
}

Value WhileLoopAction::execute(State &state) const
{
    Value result = Value(0);
    while (!isValueNull(m_cond->execute(state)))
    {
        result = m_body->execute(state);
    }
    return result;
}

Value SystemFunctionCallFunction::execute(State &state) const
{
    try
    {
        return callNativeFunction(state, m_funcId, getArguments());
    }
    catch (RuntimeActionExecutionError e)
    {
        throwRuntimeError(getCodePosition(), e.what());
    }
    return Value();
}

Value FunctionAccessAction::execute(State &state) const
{
    if (std::optional<size_t> funcId = state.getUserFunctionIdByName(m_name); funcId.has_value())
    {
        return Value(FunctionReference{.id = (uint32_t)funcId.value(), .native = false});
    }
    for (std::pair<const std::string, StandardFunctionInfo> nativeFuncs : StandardFunctions)
    {
        if (m_name == nativeFuncs.first)
        {
            return Value(FunctionReference{.id = (uint32_t)nativeFuncs.second.functionId, .native = true});
        }
    }
    return {};
}

Value UnaryOperationAction::execute(State &state) const
{
    Value v = getArgument(0)->execute(state);
    if (v.index() != ValueType::Integer)
    {
        throwRuntimeError(getCodePosition(), "Expected integer type for unary operation");
    }
    switch (m_op)
    {
    case Operator::Not:
        return !getValueAsInt(v);
    case Operator::Negate:
        return -getValueAsInt(v);
    case Operator::BitNot:
        return ~getValueAsInt(v);
    default:
        throwRuntimeError(getCodePosition(), "Unknown unary operation");
    }
    return Value();
}
