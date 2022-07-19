#include "pool.h"

//#include <iostream>

enum class CellStatus
{
    EMPTY,
    SEPARATED,
    FILLED
};

std::size_t pow2(const unsigned val)
{
    return 1u << val;
}

std::size_t log2upper(const unsigned val)
{
    std::size_t k = std::log2(val);
    if (pow2(k) < val) {
        ++k;
    }
    return k;
}

std::size_t get_number_of_level(const std::size_t n)
{
    std::size_t right = 1, delay = 2, level = 0;
    while (n > right) {
        ++level;
        right += delay;
        delay <<= 1;
    }
    return level;
}

std::size_t get_index(const std::size_t n, const unsigned max_p)
{
    std::size_t level = get_number_of_level(n);
    return (n - pow2(level)) * pow2(max_p - level);
    //    std::size_t start_pos_at_level = pow2(level);
    //    std::size_t pos_at_level = n - start_pos_at_level;
    //    std::size_t duration_at_level = pow2(max_p - level);
    //    return pos_at_level * duration_at_level;
}

/*
std::ostream & operator<<(std::ostream & os, enum CellStatus c)
{
    char const * const colours_str[] = {"E",
                                        "S",
                                        "F"};
    return os << colours_str[static_cast<int>(c)];
}

template <typename T>
void print_arr(std::vector<T> & arr)
{
    for (std::size_t i = 1; i < arr.size(); ++i) {
        std::cout << arr[i] << "\t";
    }
    std::cout << "\n";
}
 */

PoolBinaryTree::PoolBinaryTree(const unsigned min_p, const unsigned max_p)
    : min_p(min_p)
    , max_p(max_p)
    , pool_empties(pow2(max_p - min_p + 1), CellStatus::EMPTY)
{
}

std::size_t PoolBinaryTree::allocate(const std::size_t n)
{
    std::optional<std::size_t> founded_place = search_empty_place(n);
    if (founded_place) {
        std::size_t place = *founded_place;
        pool_empties[place] = CellStatus::FILLED;
        place >>= 1;
        while (place > 0) {
            pool_empties[place] = CellStatus::SEPARATED;
            place >>= 1;
        }

        return get_index(*founded_place, max_p);
    }
    else {
        throw std::bad_alloc{};
    }
}

std::optional<std::size_t> PoolBinaryTree::search_empty_place(const std::size_t n)
{
    std::size_t log2up = log2upper(n);
    if (log2up > max_p) {
        throw std::bad_alloc{};
    }
    std::size_t start_level = std::min(max_p - log2up, static_cast<std::size_t>(max_p - min_p)),
                start_pos = pow2(start_level),
                result_pos;
    int max_level = -1;
    bool resulted = false;
    for (std::size_t i = start_pos; i < start_pos * 2; ++i) {
        if (pool_empties[i] == CellStatus::EMPTY) {
            int checked = check_place(i);
            if (checked == -1) { // found a filled cell in path
                continue;
            }
            if (!resulted || max_level < checked) {
                resulted = true;
                max_level = checked;
                result_pos = i;
            }
        }
    }
    if (!resulted) {
        return {};
    }
    return result_pos;
}

int PoolBinaryTree::check_place(std::size_t index)
{
    int result = 0;
    while (index > 0) {
        if (pool_empties[index] == CellStatus::SEPARATED && result == 0) {
            result = index;
        }
        if (pool_empties[index] == CellStatus::FILLED) {
            return -1;
        }

        index >>= 1;
    }
    return result;
}

void PoolBinaryTree::deallocate(const std::size_t index)
{
    std::size_t v = 1;
    std::size_t deallocated_pos = -1;
    while (v < pool_empties.size()) {
        if (get_index(v, max_p) == index) {
            switch (pool_empties[v]) {
            case CellStatus::FILLED:
                deallocated_pos = v;
                break;
            case CellStatus::SEPARATED:
                v *= 2;
                continue;
            case CellStatus::EMPTY:
                return;
            }
            break;
        }
        else {
            v *= 2;
            v = v + (get_index(v + 1, max_p) > index ? 0 : 1);
        }
    }

    while (deallocated_pos > 0) {
        if (pool_empties[deallocated_pos] == CellStatus::FILLED ||
            (pool_empties[deallocated_pos] == CellStatus::SEPARATED &&
             pool_empties[deallocated_pos * 2] == CellStatus::EMPTY &&
             pool_empties[deallocated_pos * 2 + 1] == CellStatus::EMPTY)) {

            pool_empties[deallocated_pos] = CellStatus::EMPTY;
        }

        deallocated_pos >>= 1;
    }
}

// ---------------------------------------------------------------------------------------

PoolAllocator::PoolAllocator(const unsigned int min_p, const unsigned int max_p)
    : storage(pow2(max_p))
    , poolBinaryTree(min_p, max_p)
{
}

void * PoolAllocator::allocate(const std::size_t n)
{
    std::size_t place = poolBinaryTree.allocate(n);
    return &storage[place];
}

void PoolAllocator::deallocate(const void * ptr)
{
    std::size_t index;
    bool found = false;
    for (std::size_t i = 0; i < storage.size(); ++i) {
        if (&storage[i] == ptr) {
            found = true;
            index = i;
            break;
        }
    }
    if (found) {
        poolBinaryTree.deallocate(index);
    }
}
