#ifndef ISLIST_ITERATOR_BFAD5339_3411_49BD_9EC7_3F1C3E3C9A84
#define ISLIST_ITERATOR_BFAD5339_3411_49BD_9EC7_3F1C3E3C9A84
#include <iterator>

namespace uit {
template <typename T, auto Next>
struct islist_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    explicit constexpr islist_iterator(pointer item) {
        current = item;
    }

    [[nodiscard]]
    constexpr reference operator*() noexcept {
        return *current;
    }

    [[nodiscard]]
    constexpr reference operator*() const noexcept {
        return *current;
    }

    [[nodiscard]]
    constexpr pointer operator->() noexcept {
        return current;
    }

    [[nodiscard]]
    constexpr pointer operator->() const noexcept {
        return current;
    }

    constexpr islist_iterator &operator++() noexcept {
        current = current->*Next;
        return *this;
    }

    constexpr islist_iterator operator++(int) noexcept {
        pointer old = current;
        current = current->*Next;
        return islist_iterator{old};
    }

    constexpr bool operator==(const islist_iterator &other) const noexcept {
        return current == other.current;
    }

   private:
    pointer current{nullptr};
};
} // namespace uit
#endif /* islist_iterator.hpp */