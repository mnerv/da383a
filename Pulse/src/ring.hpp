/**
 * @file   ring.hpp
 * @author Pratchaya Khansomboon (pratchaya.k.git@gmail.com)
 * @brief  Basic fixed size ring buffer implementation with iterator.
 * @date   2022-03-20
 *
 * @copyright Copyright (c) 2022
 */
#pragma once

#include <cstdint>
#include <iterator>
#include <iostream>

namespace nrv {
// Fixed size ring buffer, FIFO
template <typename T, std::size_t SIZE>
class ring {
  public:
    template <typename pointer_type, typename reference_type>
    struct iterator_base {
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using pointer           = pointer_type;
        using reference         = reference_type;

        constexpr iterator_base(pointer buffer, std::size_t const& ptr,
                                std::size_t const& max, std::size_t const& end)
            : m_buffer(buffer), m_ptr(ptr), m_max(max), m_end(end) {}

        constexpr auto operator*() const -> reference { return m_buffer[m_ptr]; }
        constexpr auto operator->() -> pointer { return &m_buffer[m_ptr]; }

        constexpr auto operator++() -> iterator_base& {    // prefix increment
            m_ptr = (m_ptr + m_max - 1) % m_max;
            return *this;
        }
        constexpr auto operator++(int) -> iterator_base {  // postfix increment
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        constexpr friend auto operator==(iterator_base const& a, iterator_base const& b) -> bool {
            return a.m_ptr == b.m_ptr;
        }
        constexpr friend auto operator!=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a == b);
        }

        // bidirectional iterator requirements
        constexpr auto operator--() -> iterator_base& {
            m_ptr = (m_ptr + 1) % m_max;
            return *this;
        }
        constexpr auto operator--(int) -> iterator_base {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        // random access iterator requirements
        constexpr auto operator+=(difference_type const& n) -> iterator_base& {
            if ((n > 0 && n > difference_type(m_max - 1)) ||
                (n < 0 && -n > difference_type(m_max - 1)))  // sets the iterator to end iterator
                m_ptr = (m_ptr + m_max + m_max - 1) % m_max;
            else                                                        // do normal addition with wrap
                m_ptr = (m_ptr + m_max + n) % m_max;
            return *this;
        }

        constexpr auto operator+(difference_type const& n) -> iterator_base {
            if ((n > 0 && n > m_max - 1) || (n < 0 && -n > m_max - 1))  // sets the iterator to end iterator
                m_ptr = (m_ptr + m_max + m_max - 1) % m_max;
            else                                                        // do normal addition with wrap
                m_ptr = (m_ptr + m_max + n) % m_max;
            return *this;
        }
        constexpr auto operator-(difference_type const& n) -> iterator_base {
            return (*this) + (-n);
        }

        constexpr auto operator-=(difference_type const& n) -> iterator_base& {
            return (*this) += (-n);
        }

        constexpr friend auto operator-(iterator_base const& a, iterator_base const& b) -> difference_type {
            // calcuate the distance to the end iterator
            auto a_end = a.dist_to_end();
            auto b_end = b.dist_to_end();
            return a_end - b_end;
        }

        constexpr auto operator[](difference_type const& n) -> reference {
            if ((n > 0 && n > m_max - 1) || (n < 0 && -n > m_max - 1)) // offset the pointer with wrap
                return m_buffer[(m_ptr + m_max + n) % m_max];
            else                                                       // deference the unused part of the memory,
                return m_buffer[(m_ptr + m_max + m_max - 1) % m_max];  // i.e the place where end iterator is pointed at
        }

        constexpr friend auto operator<(iterator_base const& a, iterator_base const& b) -> bool {
            return b - a > 0;
        }
        constexpr friend auto operator>(iterator_base const& a, iterator_base const& b) -> bool {
            return b < a;
        }
        constexpr friend auto operator>=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a < b);
        }
        constexpr friend auto operator<=(iterator_base const& a, iterator_base const& b) -> bool {
            return !(a > b);
        }

        constexpr auto ptr() const -> std::size_t { return m_ptr; };
      private:
        // FIXME: Find a better way to get the distance from the current ptr to the end, O(n)
        auto dist_to_end() const -> difference_type {
            auto start = m_ptr;
            auto dist = 0;
            while(start != m_end) {
                dist++;
                start = (start + m_max - 1) % m_max;
            }
            return dist;
        }

      private:
        pointer     m_buffer;
        std::size_t m_ptr;
        std::size_t m_max;
        std::size_t m_end;
    };

    using iterator       = iterator_base<T*, T&>;
    using const_iterator = iterator_base<T const*, T const&>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    auto begin() -> iterator { return iterator(m_buffer, m_tail, m_max, m_head); }
    auto end()   -> iterator {
        auto const e = m_head == m_tail ? (m_head + 1) % m_max : m_head;
        return iterator(m_buffer, e, m_max, e);
    }
    auto begin() const -> const_iterator { return const_iterator(m_buffer, m_tail, m_max, m_head); }
    auto end()   const -> const_iterator { 
        auto const e = m_head == m_tail ? (m_head + 1) % m_max : m_head;
        return const_iterator(m_buffer, e, m_max, e);
    }
    auto cbegin() const -> const_iterator { return begin(); }
    auto cend()   const -> const_iterator { return end();   }

    auto rbegin() -> reverse_iterator { return reverse_iterator(end()); }
    auto rend()   -> reverse_iterator { return reverse_iterator(begin()); }
    auto crbegin() const -> const_reverse_iterator { return const_reverse_iterator(cend()); }
    auto crend()   const -> const_reverse_iterator { return const_reverse_iterator(cbegin()); }

  public:
    auto enq(T const& value) -> void {
        m_buffer[m_head] = value;
        m_head = ring::dec_wrap(m_head, m_max);
        if (m_head == m_tail)
            m_tail = ring::dec_wrap(m_tail, m_max);
        if (m_size < m_max)
            ++m_size;
    }
    auto enq_keep(T const& value) -> void {
        if (m_size == m_max - 1) return;
        enq(value);
    }
    auto deq() -> T {
        auto const ret = m_buffer[m_tail];
        if (m_tail != m_head) {
            m_tail = ring::dec_wrap(m_tail, m_max);
            --m_size;
        }
        return ret;
    }

    constexpr auto capacity() const -> std::size_t { return SIZE; }
    auto size() const -> std::size_t { return m_size; }
    auto back() -> T { return m_buffer[m_tail]; }
    auto front() -> T { return m_buffer[(m_head + 1) % m_max]; }

    using ref_type       = T&;
    using const_ref_type = T const&;
    auto operator[](std::size_t const& offset) -> ref_type { return at_back(offset); }
    auto operator[](std::size_t const& offset) const -> const_ref_type { return at_back(offset); }

    auto at_front(std::size_t const& offset) -> ref_type {
        return m_buffer[index_front(offset)];
    }
    auto at_front(std::size_t const& offset) const -> const_ref_type {
        return m_buffer[index_front(offset)];
    }

    auto at_back(std::size_t const& offset) -> ref_type {
        return m_buffer[index_back(offset)];
    }
    auto at_back(std::size_t const& offset) const -> const_ref_type {
        return m_buffer[index_back(offset)];
    }

    auto index_front(std::size_t const& offset) const -> std::size_t {
        return (m_tail + m_max - offset) % m_max;
    }
    auto index_back(std::size_t const& offset) const -> std::size_t {
        auto index = m_size == 0 ? m_head : m_head + 1;
        return (index + m_max + offset) % m_max;
    }

  private:
    static auto inc_wrap(std::size_t const& value, std::size_t const& max) -> std::size_t {
        return (value + 1) % max;
    }
    static auto dec_wrap(std::size_t const& value, std::size_t const& max) -> std::size_t {
        return (value + max - 1) % max;
    }

  private:
    T           m_buffer[SIZE + 1];  // allocate extra space for end iterator
    std::size_t m_max   = SIZE + 1;
    std::size_t m_head  = 0;
    std::size_t m_tail  = 0;
    std::size_t m_size  = 0;
};
}  // namespace nrv

