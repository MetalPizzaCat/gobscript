#pragma once
#include "Memory.hpp"

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