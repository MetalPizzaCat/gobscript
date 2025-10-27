#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <map>
#include <variant>
#include <string.h>
#include <optional>

#include <unistd.h>    /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>  /* for wait */

static const std::regex ArgumentRegex = std::regex("(\\S+)");

enum OperationType
{
    Get,
    Set,
    Equals,
    NotEquals,
};
class MemoryNode
{
public:
    void insert(MemoryNode *node)
    {
        if (node != nullptr)
        {
            MemoryNode *prevNext = m_next;
            m_next = node;
            node->m_next = prevNext;
        }
    }

    void eraseNext()
    {
        if (m_next != nullptr)
        {
            m_next = m_next->m_next;
        }
    }

    void pushBack(MemoryNode *node)
    {
        if (node == nullptr)
        {
            // don't pollute the memory
            return;
        }
        MemoryNode *curr = this;
        while (curr->m_next != nullptr)
        {
            curr = curr->m_next;
        }
        curr->m_next = node;
    }

private:
    MemoryNode *m_next;
};

class StringNode : public MemoryNode
{
public:
    explicit StringNode(std::string const &val) : m_value(val) {}
    explicit StringNode() {}

    std::string const &getValue() { return m_value; }

private:
    std::string m_value;
};

class ArrayNode : public MemoryNode
{
};

using Value = std::variant<int64_t, StringNode *, ArrayNode *>;

std::string convertValueToString(Value const &val)
{
    switch (val.index())
    {
    case 0:
        return std::to_string(std::get<int64_t>(val));
    case 1:
        return std::get<StringNode *>(val)->getValue();
    default:
        throw std::runtime_error("Rest of value handling not implemented");
    }
}

class State
{
public:
    StringNode *createString(std::string const &base)
    {
        StringNode *node = new StringNode(base);
        m_root.pushBack(node);
        return node;
    }

    // std::vector<Value> *createArray()
    // {
    //     m_arrays.push_back(std::make_unique<std::vector<Value>>({}));
    //     return m_arrays.back().get();
    // }

private:
    MemoryNode m_root;
};

class Action
{
public:
    virtual Value execute(State &state) = 0;

    void addArgument(std::unique_ptr<Action> action)
    {
        m_arguments.push_back(std::move(action));
    }

private:
    std::vector<std::unique_ptr<Action>> m_arguments;
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
    Value execute(State &state)
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
    Value execute(State &state) override
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
    Value execute(State &state) override
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

class CommandCall : public Action
{
public:
    explicit CommandCall(std::unique_ptr<Action> commandName, std::vector<std::unique_ptr<Action>> arguments) : m_commandName(std::move(commandName)),
                                                                                                                m_arguments(std::move(arguments)) {}

    explicit CommandCall(std::unique_ptr<Action> commandName) : m_commandName(std::move(commandName)) {}

    Value execute(State &state) override
    {
        std::string programName = convertValueToString(m_commandName->execute(state));
        std::vector<std::string> argsV;
        for (std::unique_ptr<Action> const &arg : m_arguments)
        {
            argsV.push_back(convertValueToString(arg->execute(state)));
        }
        std::vector<const char *> args;
        for (std::string const &arg : argsV)
        {
            args.push_back(arg.c_str());
        }
        // evaluate the arguments
        pid_t pid = fork();
        if (pid == 0)
        {
            // const char const *inputs[] = {"hi", "bye", 0};
            // execv(m_command.c_str(), (char *const *)&inputs);
            execv(programName.c_str(), (char *const *)args.data());
            exit(127);
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            return Value(status);
        }
    }

private:
    std::unique_ptr<Action> m_commandName;
    std::vector<std::unique_ptr<Action>> m_arguments;
};

void consumeCharacter(char character, std::string::const_iterator &start, std::string::const_iterator end, std::string const &errorMessage)
{
    if (start == end || *start != character)
    {
        // TODO: REPLACE WITH PROPER ERROR HANDLING PLEASE
        std::cerr << errorMessage << std::endl;
        exit(EXIT_FAILURE);
    }
}

bool expectString(std::string const &expected, std::string::const_iterator start, std::string::const_iterator end)
{
    for (size_t i = 0; i < expected.size(); i++)
    {
        if (start + i == end || expected[i] != *(start + i))
        {
            return false;
        }
    }
    // make sure there is a separator afterwards
    if ((start + expected.size()) != end)
    {
        char c = *(start + expected.size());
        if (c != ' ' && c != ')')
        {
            return false;
        }
    }
    return true;
}

void skipWhitespace(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    while (start != end && *start == ' ')
    {
        start++;
    }
}
std::string parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    while (start != end && (std::isalnum(*start) || *start == '-' || *start == '='))
    {
        result += *start;
        start++;
    }
    return result;
}

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    for (; start != end && *start != ' ' && *start != '(' && *start != ')'; start++)
    {
        result += *start;
    }
    return std::make_unique<GetConstStringAction>(result);
}

std::optional<OperationType> parseOperationType(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    return {};
}

std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> actions;
    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        if (std::unique_ptr<Action> act = parseAction(start, end); act != nullptr)
        {
            actions.push_back(std::move(act));
        }
        else
        {
            actions.push_back(parseConstString(start, end));
        }
    }
    return actions;
}
std::unique_ptr<CommandCall> parseCommandCall(std::unique_ptr<Action> commandNameAction, std::string::const_iterator &start, std::string::const_iterator end)
{
    std::string::const_iterator it = start;
    std::vector<std::unique_ptr<Action>> args = parseArguments(start, end);
    std::unique_ptr<CommandCall> exec = std::make_unique<CommandCall>(std::move(commandNameAction), std::move(args));
    skipWhitespace(start, end);
    consumeCharacter(')', start, end, "Expected ')'");
    start = it;
    return exec;
}

std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> acts;
    while (start != end && *start != ')')
    {
        acts.push_back(parseAction(start, end));
    }
    return std::make_unique<SequenceAction>(std::move(acts));
}

std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string::const_iterator it = start;
    if (*it != '(')
    {
        return nullptr;
    }
    it++;
    // check if any preexisting action
    if (std::optional<OperationType> op = parseOperationType(start, end); op.has_value())
    {
        // call the op parser
        return nullptr;
    }
    else if (std::unique_ptr<Action> action = parseConstString(start, end); action != nullptr)
    {
        auto exec = parseCommandCall(std::move(action), it, end);
        start = it;
        return exec;
    }
    return nullptr;
}


int main(int, char **)
{
    std::string currentPath = "./";
    // (program arg1 arg2 arg3)
    std::string program = "(exec echo 1 2 banana)";
    // (let ((a 0) (b 3) (c 3)) (exec echo (get a) (get b) (get c))
    std::string::const_iterator it = program.begin();
    std::unique_ptr<Action> c = parseSequence(it, program.end());
    State state;
    c->execute(state);
    return EXIT_SUCCESS;
    int pipefd[2];
    pipe(pipefd);
    pid_t pid = fork();
    if (pid == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        dup2(pipefd[1], 2);
        close(pipefd[1]);
        static char *argv[] = {"echo", "Foo is my name.", NULL};
        execv("/bin/echo", (argv));
        exit(127);
    }
    else
    {
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int status;
        close(pipefd[1]);
        while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
        {
            std::cout << buffer << std::endl;
        }
        waitpid(pid, &status, 0);
        std::cout << "Hello, from gsh!\n";
    }
}
