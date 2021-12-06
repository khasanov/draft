#pragma once

#include <string>
#include <variant>

namespace raft::object {

using Null = std::monostate;
using Boolean = bool;
using String = std::string;
using Number = double;
using Object = std::variant<Null, Boolean, String, Number>;

std::string obj2str(const Object &obj);
bool isTruthy(const Object &obj);
bool isEqual(const Object &a, const Object &b);

class NodePool {
public:
    NodePool() = default;
    ~NodePool();

    void reset();

    inline void *allocate(std::size_t size)
    {
        size = (size + 7) & ~std::size_t{7};
        if (_ptr and (_ptr + size < _end)) {
            void *addr = _ptr;
            _ptr += size;
            return addr;
        }
        return allocateHelper(size);
    }

private:
    void *allocateHelper(std::size_t size);

    NodePool(const NodePool &other) = delete;
    void operator=(const NodePool &other) = delete;

    char **_blocks = nullptr;
    int _allocatedBlocks = 0;
    int _blockCount = -1;
    char *_ptr = nullptr;
    char *_end = nullptr;
};

class NodeBase {
public:
    NodeBase() = default;
    virtual ~NodeBase() = default;

    void *operator new(std::size_t size, NodePool *pool);
    void operator delete(void *);
    void operator delete(void *, NodePool *);

private:
    NodeBase(const NodeBase &other) = delete;
    void operator=(const NodeBase &other) = delete;
};
}  // namespace raft::object
