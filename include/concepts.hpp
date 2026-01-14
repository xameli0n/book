#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T container) {
    { container.begin() } -> std::input_iterator;
    { container.end() } -> std::sentinel_for<decltype(container.begin())>;
    requires std::convertible_to<decltype(*container.begin()), Book>;
};

template <typename I>
concept BookIterator = std::input_iterator<I> && requires(I it) {
    { *it } -> std::convertible_to<Book>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = std::predicate<P, Book>;

template <typename C>
concept BookComparator = requires(C comp, Book a, Book b) {
    { comp(a, b) } -> std::convertible_to<bool>;
};

}  // namespace bookdb