#pragma once

#include <cmath>
#include <cstddef>
#include <new>
#include <optional>
#include <vector>

enum class CellStatus;

class PoolBinaryTree
{
private:
    const unsigned min_p;
    const unsigned max_p;
    std::vector<CellStatus> pool_empties;

public:
    PoolBinaryTree(const unsigned min_p, const unsigned max_p);
    std::size_t allocate(const std::size_t n);
    std::optional<std::size_t> search_empty_place(const std::size_t n);
    int check_place(std::size_t index);
    void deallocate(const std::size_t index);
};

class PoolAllocator
{
private:
    std::vector<std::byte> storage;
    PoolBinaryTree poolBinaryTree;

public:
    PoolAllocator(const unsigned min_p, const unsigned max_p);

    void * allocate(const std::size_t n);

    void deallocate(const void * ptr);
};
