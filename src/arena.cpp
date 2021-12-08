#include "arena.h"

#include <cstdint>
#include <cstdlib>

namespace raft::memory {

Block::Block()
{
    data = reinterpret_cast<std::byte *>(std::malloc(BlockSize));
    ptr = data;
}

Block::~Block()
{
    std::free(data);
}

std::byte *Block::aligned()
{
    return reinterpret_cast<std::byte *>((std::intptr_t(ptr) + 7) & ~7);
}

Arena::~Arena()
{
    for (auto &&block : blocks) {
        delete block;
    }
    blocks.clear();
}

void *Arena::allocate(std::size_t size)
{
    if (!blocks.empty()) {
        auto block = blocks.back();
        block->ptr = block->aligned();
        void *addr = block->ptr;
        block->ptr += size;
        if (block->ptr < block->data + Block::BlockSize) {
            return addr;
        }
    }
    blocks.emplace_back(new Block{});
    return allocate(size);
}

void *Object::operator new(std::size_t size, Arena *pool)
{
    return pool->allocate(size);
}

void Object::operator delete(void *)
{
}

void Object::operator delete(void *, Arena *)
{
}

}  // namespace raft::memory
