#pragma once

#include <memory>
#include <vector>

#include <string.h>
#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */

#include "Value.hpp"
#include "Operation.hpp"
#include "State.hpp"

class Action
{
public:
    virtual Value execute(State &state) const = 0;

    void addArgument(std::unique_ptr<Action> action)
    {
        m_arguments.push_back(std::move(action));
    }

    Action const *getArgument(size_t i) const
    {
        if (m_arguments.size() >= i)
        {
            return nullptr;
        }
        return m_arguments[i].get();
    }

private:
    std::vector<std::unique_ptr<Action>> m_arguments;
};

class BinaryOperationAction : public Action
{
public:
    explicit BinaryOperationAction(Operator op) : m_op(op)
    {
    }
    Value execute(State &state) const;

private:
    Operator m_op;
};

class GetConstNumberAction : public Action
{
public:
    Value execute(State &state) { return Value(m_value); }

private:
    int64_t m_value;
};

class GetConstStringAction : public Action
{
public:
    explicit GetConstStringAction(std::string const &val) : m_value(val) {}
    Value execute(State &state) const override
    {
        return state.createString(m_value);
    }

private:
    std::string m_value;
};

/// @brief Executes all given actions in a list
class SequenceAction : public Action
{
public:
    explicit SequenceAction(std::vector<std::unique_ptr<Action>> actions) : m_actions(std::move(actions)) {}
    Value execute(State &state) const override
    {
        for (size_t i = 0; i < m_actions.size(); i++)
        {
            m_actions[i]->execute(state);
        }
        return Value(0);
    }

    void addAction(std::unique_ptr<Action> action)
    {
        m_actions.push_back(std::move(action));
    }

private:
    std::vector<std::unique_ptr<Action>> m_actions;
};

class VariableBlock : public Action
{
public:
    Value execute(State &state) const override
    {
        // push variables onto the state
        // execute contents
        // pop variables from state
        return Value(0);
    }

private:
    std::map<std::string, Value> m_variables;
};

class ChangeDirectory
{
public:
private:
    std::string m_path;
};

class CommandCallAction : public Action
{
public:
    explicit CommandCallAction(std::unique_ptr<Action> commandName, std::vector<std::unique_ptr<Action>> arguments) : m_commandName(std::move(commandName)),
                                                                                                                      m_arguments(std::move(arguments)) {}

    explicit CommandCallAction(std::unique_ptr<Action> commandName) : m_commandName(std::move(commandName)) {}

    Value execute(State &state) const override
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

private:
    std::unique_ptr<Action> m_commandName;
    std::vector<std::unique_ptr<Action>> m_arguments;
};
