#include "object.h"

#include <stdexcept>

namespace raft::object {

std::string obj2str(const Object &obj)
{
    auto visitor = [](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        std::string ret;
        if constexpr (std::is_same_v<T, String>) {
            ret = arg;
        } else if constexpr (std::is_same_v<T, Number>) {
            ret = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, Boolean>) {
            ret = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, Null>) {
            ret = "nil";
        } else {
            throw std::runtime_error{"Unknown Object type"};
        }
        return ret;
    };
    return std::visit(visitor, obj);
}

// false and nil are falsey, and everything else is truthy
bool isTruthy(const Object &obj)
{
    auto visitor = [](auto &&arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, String>) {
            return true;
        } else if constexpr (std::is_same_v<T, Number>) {
            return true;
        } else if constexpr (std::is_same_v<T, Boolean>) {
            return arg;
        } else if constexpr (std::is_same_v<T, Null>) {
            return false;
        } else {
            throw std::runtime_error{"Unknown Object type"};
        }
        return true;
    };
    return std::visit(visitor, obj);
}

bool isEqual(const Object &a, const Object &b)
{
    if (std::holds_alternative<Null>(a) and std::holds_alternative<Null>(b)) {
        return true;
    }
    if (std::holds_alternative<Null>(a)) {
        return false;
    }
    return a == b;
}

NodePool::~NodePool()
{
    if (_blocks) {
        for (int i = 0; i < _allocatedBlocks; ++i) {
            if (char *b = _blocks[i]) {
                std::free(b);
            }
        }
        std::free(_blocks);
    }
}

void NodePool::reset()
{
    _blockCount = -1;
    _ptr = _end = nullptr;
}

void *NodePool::allocateHelper(std::size_t size)
{
    constexpr std::size_t DefaultBlockSize = 8 * 1024;
    constexpr std::size_t DefaultBlockCount = 8;

    std::size_t currentBlockSize = DefaultBlockSize;
    while (size >= currentBlockSize) {
        currentBlockSize *= 2;
    }

    if (++_blockCount == _allocatedBlocks) {
        if (!_allocatedBlocks) {
            _allocatedBlocks = DefaultBlockCount;
        } else {
            _allocatedBlocks *= 2;
        }

        _blocks = reinterpret_cast<char **>(std::realloc(_blocks, sizeof(char *) * std::size_t(_allocatedBlocks)));

        for (int index = _blockCount; index < _allocatedBlocks; ++index) {
            _blocks[index] = nullptr;
        }
    }

    char *&block = _blocks[_blockCount];

    if (!block) {
        block = reinterpret_cast<char *>(std::malloc(currentBlockSize));
    }

    _ptr = block;
    _end = _ptr + currentBlockSize;

    void *addr = _ptr;
    _ptr += size;
    return addr;
}

void *NodeBase::operator new(std::size_t size, NodePool *pool)
{
    return pool->allocate(size);
}

void NodeBase::operator delete(void *)
{
}

void NodeBase::operator delete(void *, NodePool *)
{
}

}  // namespace raft::object
