#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T container) {
    requires std::same_as<typename T::value_type, Book>;

    { container.begin() } -> std::input_iterator;
    { container.end() } -> std::sentinel_for<decltype(container.begin())>;
    { *container.begin() } -> std::convertible_to<Book>;

    { container.size() } -> std::convertible_to<std::size_t>;
    { container.empty() } -> std::convertible_to<bool>;
};

template <typename I>
concept BookIterator = std::input_iterator<I> && requires(I it) {
    { *it } -> std::convertible_to<Book>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I> && BookIterator<I>;

template <typename P>
concept BookPredicate = std::predicate<P, Book>;

template <typename C>
concept BookComparator = requires(C comp, const Book &a, const Book &b) {
    { comp(a, b) } -> std::convertible_to<bool>;
};

}  // namespace bookdb