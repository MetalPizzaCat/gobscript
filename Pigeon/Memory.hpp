#pragma once
#include <string>

class MemoryNode
{
public:
    explicit MemoryNode() = default;
    void insert(MemoryNode *node);

    /// @brief Remove the next node and attach it's child as the current child
    void eraseNext();

    /// @brief Get next node in memory
    /// @return
    MemoryNode *getNext() { return m_next; }

    void pushBack(MemoryNode *node);

    void increaseRefCount();

    void decreaseRefCount();

    int32_t getRefCount() const { return m_refCount; }

    virtual size_t getLen() const { return 0; }

    /**
     * @brief Should be deleted by the garbage collector or not
     */
    bool isDead() const { return m_dead || m_refCount <= 0; }

    virtual ~MemoryNode() = default;

private:
    MemoryNode *m_next = nullptr;
    /// @brief Is marked for deletion by garbage collector?
    bool m_dead = false;

    /// @brief How many references to this object exist. Once it zero object should be deleted
    int32_t m_refCount = 0;
};

class StringNode : public MemoryNode
{
public:
    explicit StringNode(std::string const &val) : m_value(val) {}
    explicit StringNode() {}

    size_t getLen() const override { return m_value.size(); }

    std::string &getValue() { return m_value; }

    virtual ~StringNode() {}

private:
    std::string m_value;
};
