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
    explicit Action(std::string::const_iterator const &it) : m_codePtr(it) {}
    explicit Action(std::string::const_iterator const &it, std::vector<std::unique_ptr<Action>> args) : m_codePtr(it), m_arguments(std::move(args)) {}
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

    /// @brief Get position in code from which this action was parsed
    /// @return Iterator pointing to the position in code from which this action was parsed
    std::string::const_iterator const &getCodePosition() const { return m_codePtr; }

private:
    std::vector<std::unique_ptr<Action>> m_arguments;
    std::string::const_iterator m_codePtr;
};

class BinaryOperationAction : public Action
{
public:
    explicit BinaryOperationAction(std::string::const_iterator const &it,
                                   Operator op,
                                   std::vector<std::unique_ptr<Action>> args) : m_op(op),
                                                                                Action(it, std::move(args))
    {
    }
    Value execute(State &state) const;

private:
    Operator m_op;
};

class UnaryOperationAction : public Action
{
public:
    explicit UnaryOperationAction(std::string::const_iterator const &it, Operator op, std::vector<std::unique_ptr<Action>> args) : m_op(op), Action(it, std::move(args))
    {
    }
    Value execute(State &state) const;

private:
    Operator m_op;
};

class AssignOperationAction : public Action
{
public:
    explicit AssignOperationAction(std::string::const_iterator const &it, Operator op,
                                   std::string const &variableName,
                                   std::unique_ptr<Action> value) : m_op(op),
                                                                    m_name(variableName),
                                                                    m_value(std::move(value)),
                                                                    Action(it)
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
    explicit GetConstNumberAction(std::string::const_iterator const &it, int64_t val) : m_value(val), Action(it) {}
    Value execute(State &state) const override { return Value((int64_t)m_value); }

private:
    int64_t m_value;
};

class GetConstStringAction : public Action
{
public:
    explicit GetConstStringAction(std::string::const_iterator const &it, std::string const &val) : m_value(val), Action(it) {}
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
    explicit SequenceAction(std::string::const_iterator const &it, std::vector<std::unique_ptr<Action>> actions) : Action(it, std::move(actions)) {}
    Value execute(State &state) const override;
};

class BranchAction : public Action
{
public:
    explicit BranchAction(std::string::const_iterator const &it, std::unique_ptr<Action> cond,
                          std::unique_ptr<Action> thenBranch,
                          std::unique_ptr<Action> elseBranch) : m_cond(std::move(cond)),
                                                                m_then(std::move(thenBranch)),
                                                                m_else(std::move(elseBranch)),
                                                                Action(it)
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
    explicit VariableAccessAction(std::string::const_iterator const &it, std::string const &name) : m_name(name), Action(it) {}
    Value execute(State &state) const override;

private:
    std::string m_name;
};

class FunctionAccessAction : public Action
{
public:
    explicit FunctionAccessAction(std::string::const_iterator const &it, std::string const &name) : m_name(name), Action(it) {}
    Value execute(State &state) const override;

private:
    std::string m_name;
};

class VariableBlockAction : public Action
{
public:
    explicit VariableBlockAction(std::string::const_iterator const &it,
                                 std::map<std::string, std::unique_ptr<Action>> variables,
                                 std::unique_ptr<Action> body) : m_body(std::move(body)),
                                                                 m_variables(std::move(variables)), Action(it) {}

    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_body;
    std::map<std::string, std::unique_ptr<Action>> m_variables;
};

class CommandCallAction : public Action
{
public:
    explicit CommandCallAction(std::string::const_iterator const &it,
                               std::unique_ptr<Action> commandName,
                               std::vector<std::unique_ptr<Action>> arguments) : m_commandName(std::move(commandName)),
                                                                                 m_arguments(std::move(arguments)),
                                                                                 Action(it) {}

    explicit CommandCallAction(std::string::const_iterator const &it, std::unique_ptr<Action> commandName) : m_commandName(std::move(commandName)), Action(it) {}

    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_commandName;
    std::vector<std::unique_ptr<Action>> m_arguments;
};

class CreateArrayAction : public Action
{
public:
    explicit CreateArrayAction(std::string::const_iterator const &it, std::vector<std::unique_ptr<Action>> items) : Action(it, std::move(items)) {}
    Value execute(State &state) const override;
};

class FunctionDeclarationAction : public Action
{
public:
    explicit FunctionDeclarationAction(std::string::const_iterator const &it, std::string const &name,
                                       std::unique_ptr<Action> body,
                                       std::vector<std::string> const &arguments) : m_name(name),
                                                                                    m_arguments(arguments),
                                                                                    m_body(std::move(body)),
                                                                                    Action(it)
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
    explicit FunctionCallAction(std::string::const_iterator const &it, std::unique_ptr<Action> functionAccess,
                                std::vector<std::unique_ptr<Action>> arguments) : m_functionAccess(std::move(functionAccess)),
                                                                                  Action(it, std::move(arguments))
    {
    }
    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_functionAccess;
};

class ForLoopAction : public Action
{
public:
    explicit ForLoopAction(std::string::const_iterator const &it,
                           std::unique_ptr<Action> init,
                           std::unique_ptr<Action> cond,
                           std::unique_ptr<Action> iter,
                           std::unique_ptr<Action> body) : m_init(std::move(init)),
                                                           m_cond(std::move(cond)),
                                                           m_iter(std::move(iter)),
                                                           m_body(std::move(body)),
                                                           Action(it) {}
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
    explicit WhileLoopAction(std::string::const_iterator const &it,
                             std::unique_ptr<Action> cond,
                             std::unique_ptr<Action> body) : m_cond(std::move(cond)),
                                                             m_body(std::move(body)),
                                                             Action(it) {}
    Value execute(State &state) const override;

private:
    std::unique_ptr<Action> m_cond;
    std::unique_ptr<Action> m_body;
};

/// @brief System function is any function considered part of the "standard library" and refers to functions stored in the state
class SystemFunctionCallFunction : public Action
{
public:
    explicit SystemFunctionCallFunction(std::string::const_iterator const &it,
                                        size_t funcId,
                                        std::vector<std::unique_ptr<Action>> args) : m_funcId(funcId),
                                                                                     Action(it, std::move(args)) {}
    Value execute(State &state) const override;

private:
    size_t m_funcId;
};