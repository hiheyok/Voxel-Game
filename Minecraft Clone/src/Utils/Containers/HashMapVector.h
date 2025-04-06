#pragma once
#include "Core/Typenames.h"

#include <vector>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include <iterator>

template <class Key, class Val>
class HashMapVector {
public:
    using value_type = Val;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <bool IsConst>
    class base_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = HashMapVector::value_type;
        using difference_type = HashMapVector::difference_type;

        using pointer = typename std::conditional<IsConst, const value_type*, value_type*>::type;
        using reference = typename std::conditional<IsConst, const value_type&, value_type&>::type;

        using internal_iterator_type = typename std::conditional<
            IsConst,
            typename std::vector<value_type>::const_iterator,
            typename std::vector<value_type>::iterator
        >::type;

        base_iterator() = default;

        explicit base_iterator(internal_iterator_type it) : current_(it) {}

        template <bool WasConst = IsConst, typename = std::enable_if_t<WasConst>>
        base_iterator(const base_iterator<false>& other) : current_(other.base()) {}

        reference operator*() const { return *current_; }
        pointer operator->() const { return current_.operator->(); } // Or &(*current_)

        reference operator[](difference_type n) const { return current_[n]; }

        base_iterator& operator++() { ++current_; return *this; } // prefix ++
        base_iterator operator++(int) { base_iterator tmp = *this; ++current_; return tmp; } // postfix ++
        base_iterator& operator--() { --current_; return *this; } // prefix --
        base_iterator operator--(int) { base_iterator tmp = *this; --current_; return tmp; } // postfix --

        base_iterator& operator+=(difference_type n) { current_ += n; return *this; }
        base_iterator& operator-=(difference_type n) { current_ -= n; return *this; }

        friend base_iterator operator+(base_iterator lhs, difference_type n) {
            lhs += n;
            return lhs;
        }
        friend base_iterator operator+(difference_type n, base_iterator rhs) {
            rhs += n;
            return rhs;
        }
        friend base_iterator operator-(base_iterator lhs, difference_type n) {
            lhs -= n;
            return lhs;
        }
        friend difference_type operator-(const base_iterator& lhs, const base_iterator& rhs) {
            return lhs.current_ - rhs.current_;
        }

        friend bool operator==(const base_iterator& lhs, const base_iterator& rhs) {
            return lhs.current_ == rhs.current_;
        }
        // C++20: Define <=> spaceship operator if desired for simplicity
        friend bool operator!=(const base_iterator& lhs, const base_iterator& rhs) {
            return !(lhs == rhs);
        }
        friend bool operator<(const base_iterator& lhs, const base_iterator& rhs) {
            return lhs.current_ < rhs.current_;
        }
        friend bool operator>(const base_iterator& lhs, const base_iterator& rhs) {
            return rhs < lhs;
        }
        friend bool operator<=(const base_iterator& lhs, const base_iterator& rhs) {
            return !(rhs < lhs);
        }
        friend bool operator>=(const base_iterator& lhs, const base_iterator& rhs) {
            return !(lhs < rhs);
        }

        internal_iterator_type base() const { return current_; }


    private:
        internal_iterator_type current_;
    };

    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;

    HashMapVector() = default;

    Val& operator[](const Key& key) {
        auto map_it = idx_.find(key);
        if (map_it != idx_.end()) {
            return arr_[map_it->second];
        } else {
            size_t new_idx = arr_.size();
            idx_[key] = new_idx;
            try {
                arr_.emplace_back(Val{});
                keys_.emplace_back(key);
                return arr_.back();
            }
            catch (...) {
                idx_.erase(key);
                throw;
            }
        }
    }

    void Erase(const Key& key) {
        auto map_it = idx_.find(key);
        if (map_it == idx_.end()) {
            return;
        }

        size_t idx_to_remove = map_it->second;
        size_t idx_last = arr_.size() - 1;

        if (idx_to_remove != idx_last) {
            const Key& key_of_last = keys_.back();

            std::swap(arr_[idx_to_remove], arr_.back());
            std::swap(keys_[idx_to_remove], keys_.back());

            idx_[key_of_last] = idx_to_remove;
        }

        arr_.pop_back();
        keys_.pop_back();

        idx_.erase(map_it);
    }

    bool Contains(const Key& key) const {
        return idx_.count(key) > 0;
    }

    const Val& At(const Key& key) const {
        auto map_it = idx_.find(key);
        if (map_it == idx_.end()) {
            throw std::out_of_range("HashMapVector::At key not found");
        }
        return arr_[map_it->second];
    }

    Val& At(const Key& key) {
        auto map_it = idx_.find(key);
        if (map_it == idx_.end()) {
            throw std::out_of_range("HashMapVector::At key not found");
        }
        return arr_[map_it->second];
    }


    size_type size() const noexcept {
        return arr_.size();
    }

    bool empty() const noexcept {
        return arr_.empty();
    }

    void clear() noexcept {
        arr_.clear();
        idx_.clear();
    }

    iterator begin() noexcept {
        return iterator(arr_.begin());
    }

    iterator end() noexcept {
        return iterator(arr_.end());
    }

    const_iterator begin() const noexcept {
        return const_iterator(arr_.cbegin());
    }

    const_iterator end() const noexcept {
        return const_iterator(arr_.cend());
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(arr_.cbegin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(arr_.cend());
    }

    std::vector<Val> GetVals() const {
        return arr_;
    }

private: 
    FastHashMap<Key, size_t> idx_;
    std::vector<Key> keys_;
    std::vector<Val> arr_;
};