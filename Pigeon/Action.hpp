#pragma once

#include <memory>
#include <vector>
#include <map>

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
    explicit Action() = default;
    explicit Action(std::vector<std::unique_ptr<Action>> args) : m_arguments(std::move(args)) {}
    virtual Value execute(State &state) const = 0;

    void addArgument(std::unique_ptr<Action> action)
    {
        m_arguments.push_back(std::move(action));
    }

    Action const *getArgument(size_t i) const
    {
        if (i < m_arguments.size())
        {
            return m_arguments[i].get();
        }
        return nullptr;
    }

    size_t getArgumentCount() const { return m_arguments.size(); }

    std::vector<std::unique_ptr<Action>> const &getArguments() const
    {
        return m_arguments;
    }

private:
    std::vector<std::unique_ptr<Action>> m_arguments;
};

class BinaryOperationAction : public Action
{
public:
    explicit BinaryOperationAction(Operator op, std::vector<std::unique_ptr<Action>> args) : m_op(op),  Action(std::move(args))
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
    explicit SequenceAction(std::vector<std::unique_ptr<Action>> actions) : Action(std::move(actions)) {}
    Value execute(State &state) const override
    {
        for (size_t i = 0; i < getArgumentCount(); i++)
        {
            auto it = getArgument(i);
            if (getArgument(i) != nullptr)
            {
                getArgument(i)->execute(state);
            }
        }
        // TODO: Make it return value of the last action
        return Value(0);
    }
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

    Value execute(State &state) const override;
    

private:
    std::unique_ptr<Action> m_commandName;
    std::vector<std::unique_ptr<Action>> m_arguments;
};
