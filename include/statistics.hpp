#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <numeric>
#include <random>
#include <ranges>
#include <span>
#include <vector>

#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

// Функция для создания гистограммы количества книг по авторам
template <BookContainerLike T, typename Comparator = TransparentStringLess>
[[nodiscard]] auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string, size_t, Comparator> histogram(comp);

    for (const auto &book : cont) {
        histogram[std::string(book.author)]++;
    }

    return histogram;
}

// Функция для вычисления среднего рейтинга по жанрам
[[nodiscard]] std::flat_map<Genre, double> calculateGenreRatings(BookIterator auto first, BookIterator auto last) {
    std::flat_map<Genre, std::pair<double, size_t>> genre_stats;

    for (auto it = first; it != last; ++it) {
        const auto &book = *it;
        Genre key = book.genre;  // Создаем lvalue для ключа
        auto &[sum, count] = genre_stats[key];
        sum += book.rating;
        count++;
    }

    std::flat_map<Genre, double> result;
    for (const auto &[genre, stats] : genre_stats) {
        const auto &[sum, count] = stats;
        Genre genre_key = genre;  // lvalue для ключа
        result[genre_key] = sum / count;
    }

    return result;
}

// Перегрузка с std::span
[[nodiscard]] std::flat_map<Genre, double> calculateGenreRatings(std::span<const Book> books) {
    return calculateGenreRatings(books.begin(), books.end());
}

// Функция для вычисления среднего рейтинга всех книг
[[nodiscard]] double calculateAverageRating(const BookContainerLike auto &books) {
    if (books.empty())
        return 0.0;

    return std::transform_reduce(books.begin(), books.end(), 0.0, std::plus<>{},
                                 [](const Book &book) { return book.rating; }) /
           books.size();
}

// Функция для случайной выборки книг
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(BookIterator auto first,
                                                                                BookIterator auto last, size_t n) {
    std::vector<std::reference_wrapper<const Book>> result;

    std::random_device rd;
    std::mt19937 g(rd());

    std::sample(first, last, std::back_inserter(result), n, g);

    return result;
}

// Перегрузка с std::span
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(std::span<const Book> books, size_t n) {
    return sampleRandomBooks(books.begin(), books.end(), n);
}

// Функция для выбора топ N книг по рейтингу
template <BookComparator Comp = comp::GreaterByRating>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> getTopNBy(const BookContainerLike auto &books, size_t n,
                                                                        Comp comp = {}) {
    std::vector<std::reference_wrapper<const Book>> refs;

    if (books.empty() || n == 0)
        return refs;

    // Создаем вектор ссылок
    refs.reserve(books.size());
    for (const auto &book : books) {
        refs.emplace_back(book);
    }

    // Берем первые n элементов по компаратору
    size_t count = std::min(n, refs.size());
    std::partial_sort(refs.begin(), refs.begin() + count, refs.end(), comp);

    // Удаляем лишние элементы
    refs.erase(refs.begin() + count, refs.end());

    return refs;
}

}  // namespace bookdb