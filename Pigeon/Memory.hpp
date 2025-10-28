#pragma once
#include <string>

class MemoryNode
{
public:
    explicit MemoryNode() = default;
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
    MemoryNode *m_next = nullptr;
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