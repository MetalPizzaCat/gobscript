#include "Memory.hpp"

void MemoryNode::insert(MemoryNode *node)
{
    if (node != nullptr)
    {
        MemoryNode *prevNext = m_next;
        m_next = node;
        node->m_next = prevNext;
    }
}

void MemoryNode::eraseNext()
{
    if (m_next != nullptr)
    {
        m_next = m_next->m_next;
    }
}

void MemoryNode::pushBack(MemoryNode *node)
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

void MemoryNode::increaseRefCount()
{
    m_refCount++;
}

void MemoryNode::decreaseRefCount()
{
    m_refCount--;
}
