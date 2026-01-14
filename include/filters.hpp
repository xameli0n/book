#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

// Фабрики предикатов

// Проверка года в диапазоне
auto YearBetween(int min_year, int max_year) {
    return [min_year, max_year](const Book &book) { return book.year >= min_year && book.year <= max_year; };
}

// Проверка рейтинга выше порога
auto RatingAbove(double threshold) {
    return [threshold](const Book &book) { return book.rating > threshold; };
}

// Проверка жанра
auto GenreIs(Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

// Комбинированные предикаты с fold expressions
// all_of: все предикаты должны быть true
template <BookPredicate... Preds>
auto all_of(Preds... preds) {
    return [preds...](const Book &book) { return (preds(book) && ...); };
}

// any_of: хотя бы один предикат true
template <BookPredicate... Preds>
auto any_of(Preds... preds) {
    return [preds...](const Book &book) { return (preds(book) || ...); };
}

template <BookPredicate Pred>
std::vector<std::reference_wrapper<const Book>> filterBooks(BookIterator auto first, BookIterator auto last,
                                                            Pred pred) {
    std::vector<std::reference_wrapper<const Book>> result;

    for (auto it = first; it != last; ++it) {
        if (pred(*it)) {
            result.emplace_back(*it);
        }
    }

    return result;
}

}  // namespace bookdb