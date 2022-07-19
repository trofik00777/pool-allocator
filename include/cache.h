#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <new>
#include <ostream>

template <class KeyProvider>
struct Element
{
    bool also_used;
    KeyProvider * elem;

    Element(KeyProvider * elem_, bool also_used_ = false)
        : also_used(also_used_)
        , elem(elem_)
    {
    }
};

template <class Key, class KeyProvider, class Allocator>
class Cache
{
public:
    template <class... AllocArgs>
    Cache(const std::size_t cache_size, AllocArgs &&... alloc_args)
        : m_max_size(cache_size)
        , m_alloc(std::forward<AllocArgs>(alloc_args)...)
    {
    }

    std::size_t size() const
    {
        return queue.size();
    }

    bool empty() const
    {
        return size() == 0;
    }

    template <class T>
    T & get(const Key & key);

    std::ostream & print(std::ostream & strm) const;

    friend std::ostream & operator<<(std::ostream & strm, const Cache & cache)
    {
        return cache.print(strm);
    }

private:
    const std::size_t m_max_size;
    Allocator m_alloc;
    std::list<Element<KeyProvider>> queue;
};

template <class Key, class KeyProvider, class Allocator>
template <class T>
inline T & Cache<Key, KeyProvider, Allocator>::get(const Key & key)
{
    auto it = std::find_if(queue.begin(), queue.end(), [&key](Element<KeyProvider> & element) {
        return *element.elem == key;
    });
    if (it != queue.end()) {
        Element<KeyProvider> & elem = *it;
        elem.also_used = true;
        return *static_cast<T *>(elem.elem);
    }
    else {
        if (size() >= m_max_size) {
            while (queue.back().also_used) {
                queue.splice(queue.begin(), queue, --queue.end());
                queue.front().also_used = false;
            }
            m_alloc.destroy(queue.back().elem);
            queue.pop_back();
        }
        queue.push_front(Element<KeyProvider>(m_alloc.template create<T>(T(key))));
        return *static_cast<T *>(queue.front().elem);
    }
}

template <class Key, class KeyProvider, class Allocator>
inline std::ostream & Cache<Key, KeyProvider, Allocator>::print(std::ostream & strm) const
{
    return strm << "<empty>\n";
}
