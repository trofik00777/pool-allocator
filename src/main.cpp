#include "allocator.h"
#include "cache.h"

#include <iostream>
#include <string>

namespace {

constexpr unsigned upper_bin_power(const std::size_t n)
{
    unsigned i = 0;
    while ((1UL << i) < n) {
        ++i;
    }
    return i;
}

struct String
{
    std::string data;
    bool marked = false;

    String(const std::string & key)
        : data(key)
    {
    }

    bool operator==(const std::string & other) const
    {
        return data == other;
    }

    //    friend std::ostream & operator<<(std::ostream & strm, const String & str)
    //    {
    //        return strm << str.data;
    //    }
};

using TestCache = Cache<std::string, String, AllocatorWithPool>;

} // anonymous namespace

int main()
{
    //    PoolAllocator alloc(1, 4);
    //    auto f1 = alloc.allocate(17);
    //    alloc.print();
    //    auto f2 = alloc.allocate(1);
    //    alloc.print();
    //    alloc.allocate(9);
    //    std::cout << f1 << " " << f2 << "\n";
    ////    static_cast<int *>(f2);
    //    alloc.deallocate(f2);
    //    alloc.deallocate(f2);
    ////    alloc.deallocate(f1);
    //    alloc.print();
    const std::size_t min_power = upper_bin_power(sizeof(String));
    TestCache cache(9, min_power, upper_bin_power((1UL << min_power) * 10));
    std::string line;
    while (std::getline(std::cin, line)) {
        auto & s = cache.get<String>(line);
        if (s.marked) {
            std::cout << "known" << std::endl;
        }
        s.marked = true;
    }
    std::cout << "\n"
              << cache << std::endl;
}
