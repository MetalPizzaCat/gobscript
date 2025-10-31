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
    explicit BinaryOperationAction(Operator op, std::vector<std::unique_ptr<Action>> args) : m_op(op), Action(std::move(args))
    {
    }
    Value execute(State &state) const;

private:
    Operator m_op;
};

class AssignOperationAction : public Action
{
public:
    explicit AssignOperationAction(Operator op,
                                   std::string const &variableName,
                                   std::unique_ptr<Action> value) : m_op(op),
                                                                    m_name(variableName),
                                                                    m_value(std::move(value))
    {
    }
    Value execute(State &state) const;

private:
    std::string m_name;
    std::unique_ptr<Action> m_value;
    Operator m_op;
};
class GetConstNumberAction : public Action
{
public:
    explicit GetConstNumberAction(int64_t val) : m_value(val) {}
    Value execute(State &state) const override { return Value((int64_t)m_value); }

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
    Value execute(State &state) const override;
};

class BranchAction : public Action
{
public:
    explicit BranchAction(std::unique_ptr<Action> cond,
                          std::unique_ptr<Action> thenBranch,
                          std::unique_ptr<Action> elseBranch) : m_cond(std::move(cond)),
                                                                m_then(std::move(thenBranch)),
                                                                m_else(std::move(elseBranch))
    {
    }

    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_cond;
    std::unique_ptr<Action> m_then;
    std::unique_ptr<Action> m_else;
};

class VariableAccessAction : public Action
{
public:
    explicit VariableAccessAction(std::string const &name) : m_name(name) {}
    Value execute(State &state) const override;

private:
    std::string m_name;
};

class VariableBlockAction : public Action
{
public:
    explicit VariableBlockAction(
        std::map<std::string, std::unique_ptr<Action>> variables,
        std::unique_ptr<Action> body) : m_body(std::move(body)),
                                        m_variables(std::move(variables)) {}

    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_body;
    std::map<std::string, std::unique_ptr<Action>> m_variables;
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

class CreateArrayAction : public Action
{
public:
    explicit CreateArrayAction(std::vector<std::unique_ptr<Action>> items) : Action(std::move(items)) {}
    Value execute(State &state) const override;
};

class FunctionDeclarationAction : public Action
{
public:
    explicit FunctionDeclarationAction(std::string const &name,
                                       std::unique_ptr<Action> body,
                                       std::vector<std::string> const &arguments) : m_name(name),
                                                                                    m_arguments(arguments),
                                                                                    m_body(std::move(body))
    {
    }
    Value execute(State &state) const override;

private:
    std::string m_name;
    std::unique_ptr<Action> m_body;
    std::vector<std::string> m_arguments;
};

class FunctionCallAction : public Action
{
public:
    explicit FunctionCallAction(std::string const &name, std::vector<std::unique_ptr<Action>> arguments) : m_name(name), Action(std::move(arguments))
    {
    }
    Value execute(State &state) const override;

private:
    std::string m_name;
};

class ForLoopAction : public Action
{
public:
    explicit ForLoopAction(
        std::unique_ptr<Action> init,
        std::unique_ptr<Action> cond,
        std::unique_ptr<Action> iter,
        std::unique_ptr<Action> body) : m_init(std::move(init)),
                                        m_cond(std::move(cond)),
                                        m_iter(std::move(iter)),
                                        m_body(std::move(body)) {}
    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_init;
    std::unique_ptr<Action> m_cond;
    std::unique_ptr<Action> m_iter;
    std::unique_ptr<Action> m_body;
};

class WhileLoopAction : public Action
{
public:
    explicit WhileLoopAction(
        std::unique_ptr<Action> cond,
        std::unique_ptr<Action> body) : m_cond(std::move(cond)),
                                        m_body(std::move(body)) {}
    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_cond;
    std::unique_ptr<Action> m_body;
};