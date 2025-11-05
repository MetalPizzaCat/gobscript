#include "State.hpp"
#include "Action.hpp"
#include <algorithm>
StringNode *State::createString(std::string const &base)
{
    StringNode *node = new StringNode(base);
    m_root.pushBack(node);
    return node;
}

ArrayNode *State::createArray(std::vector<Value> const values)
{
    ArrayNode *node = new ArrayNode(values);
    m_root.pushBack(node);
    return node;
}

std::optional<Value> State::getVariableValue(std::string const &name)
{
    for (std::vector<std::map<std::string, Value>>::reverse_iterator it = m_variables.rbegin(); it != m_variables.rend(); it++)
    {
        if (it->count(name) > 0)
        {
            return it->at(name);
        }
    }
    return {};
}

std::optional<Value> State::setVariableValue(std::string const &name, Value val)
{
    for (std::vector<std::map<std::string, Value>>::reverse_iterator it = m_variables.rbegin(); it != m_variables.rend(); it++)
    {
        if (it->count(name) > 0)
        {
            (*it)[name] = val;
            return val;
        }
    }
    // if doesn't exist, create
    // could consider making this a build flag, but currently this is to make the shell work nicer
    m_variables.back()[name] = val;
    return val;
}

bool State::doesVariableExist(std::string const &name)
{
    for (std::vector<std::map<std::string, Value>>::reverse_iterator it = m_variables.rbegin(); it != m_variables.rend(); it++)
    {
        if (it->count(name) > 0)
        {
            return true;
        }
    }
    return false;
}

bool State::doesVariableExistAndOfType(std::string const &name, ValueType type)
{
    for (std::vector<std::map<std::string, Value>>::reverse_iterator it = m_variables.rbegin(); it != m_variables.rend(); it++)
    {
        if (it->count(name) > 0)
        {
            return it->at(name).index() == type;
        }
    }
    return false;
}

void State::pushVariableScope(std::map<std::string, Value> variables)
{
    m_variables.push_back(variables);
    for (std::pair<const std::string, Value> &v : variables)
    {
        switch (v.second.index())
        {
        case ValueType::String:
            std::get<StringNode *>(v.second)->increaseRefCount();
            break;
        case ValueType::Array:
            std::get<ArrayNode *>(v.second)->increaseRefCount();
            break;
        }
    }
}

void State::popVariableScope()
{
    if (m_variables.empty())
    {
        return;
    }
    for (std::pair<const std::string, Value> &var : m_variables.back())
    {
        if (var.second.index() == ValueType::Array)
        {
            std::get<ArrayNode *>(var.second)->decreaseRefCount();
        }
        else if (var.second.index() == ValueType::String)
        {
            std::get<StringNode *>(var.second)->decreaseRefCount();
        }
    }
    m_variables.pop_back();
    collectGarbage();
}

void State::addFunction(std::string const &name, std::vector<std::string> arguments, Action const *body)
{
    m_functionNames.push_back(name);
    m_functions.push_back(Function(body, arguments));
}

std::optional<Function> State::getFunction(std::string const &name) const
{
    if (std::vector<std::string>::const_iterator it = std::find(m_functionNames.begin(), m_functionNames.end(), name); it != m_functionNames.end())
    {
        return m_functions[it - m_functionNames.begin()];
    }
    return {};
}

std::optional<State::NativeFunction> State::getStandardFunction(size_t i) const
{
    if (i < m_standardFunctions.size())
    {
        return m_standardFunctions[i];
    }
    return {};
}

std::optional<Function> State::getUserFunctionById(size_t i) const
{
    if (i < m_functions.size())
    {
        return m_functions[i];
    }
    return {};
}

std::optional<std::string> State::getUserFunctionNameById(size_t i) const
{
    if (i < m_functionNames.size())
    {
        return m_functionNames[i];
    }
    return {};
}

std::optional<size_t> State::getUserFunctionIdByName(std::string const &name) const
{
    if (std::vector<std::string>::const_iterator it = std::find(m_functionNames.begin(), m_functionNames.end(), name); it != m_functionNames.end())
    {
        return it - m_functionNames.begin();
    }
    return {};
}

void State::collectGarbage()
{
    MemoryNode *prev = &m_root;
    MemoryNode *curr = m_root.getNext();
    while (curr != nullptr)
    {
        if (!curr->isDead())
        {
            prev = curr;
            curr = curr->getNext();
            continue;
        }
        // if we are deleting then prev should stay the same while
        // curr gets deleted
        MemoryNode *del = curr;
        prev->eraseNext();
        curr = prev->getNext();

        delete del;
    }
}

State::~State()
{
    MemoryNode *prev = &m_root;
    MemoryNode *curr = m_root.getNext();
    while (curr != nullptr)
    {
        // if we are deleting then prev should stay the same while
        // curr gets deleted
        MemoryNode *del = curr;
        prev->eraseNext();
        curr = prev->getNext();

        delete del;
    }
}
