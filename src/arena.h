#pragma once

#include <vector>

namespace draft::memory {

class Block {
public:
    Block();
    ~Block();

    std::byte *aligned();

    std::byte *data = nullptr;
    std::byte *ptr = nullptr;

    static constexpr std::size_t BlockSize = 8 * 1024;

private:
    Block(const Block &) = delete;
    Block &operator=(const Block &) = delete;
};

class Arena {
public:
    Arena() = default;
    ~Arena();

    void *allocate(std::size_t size);

private:
    Arena(const Arena &other) = delete;
    Arena &operator=(const Arena &other) = delete;

    std::vector<Block *> blocks;
};

class Object {
public:
    Object() = default;
    virtual ~Object() = default;

    void *operator new(std::size_t size, Arena *pool);
    void operator delete(void *);
    void operator delete(void *, Arena *);

private:
    Object(const Object &other) = delete;
    void operator=(const Object &other) = delete;
};

}  // namespace draft::memory
