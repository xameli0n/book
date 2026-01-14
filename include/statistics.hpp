#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <random>
#include <ranges>
#include <span>
#include <vector>

#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "concepts.hpp"

namespace bookdb {

// Функция для создания гистограммы количества книг по авторам
std::flat_map<std::string, size_t> buildAuthorHistogramFlat(BookContainerLike auto &&books) {
    std::flat_map<std::string, size_t> histogram;

    for (const auto &book : books) {
        histogram[std::string(book.author)]++;
    }

    return histogram;
}

// Функция для вычисления среднего рейтинга по жанрам
std::flat_map<Genre, double> calculateGenreRatings(BookIterator auto first, BookIterator auto last) {
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
std::flat_map<Genre, double> calculateGenreRatings(std::span<const Book> books) {
    return calculateGenreRatings(books.begin(), books.end());
}

// Функция для вычисления среднего рейтинга всех книг
double calculateAverageRating(BookContainerLike auto &&books) {
    if (books.empty())
        return 0.0;

    return std::accumulate(books.begin(), books.end(), 0.0,
                           [](double sum, const Book &book) { return sum + book.rating; }) /
           books.size();
}

// Функция для случайной выборки книг
std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(BookIterator auto first, BookIterator auto last,
                                                                  size_t n) {
    std::vector<std::reference_wrapper<const Book>> result;

    auto distance = std::distance(first, last);
    if (distance == 0 || n == 0)
        return result;

    // Создаем вектор индексов
    std::vector<size_t> indices(distance);
    std::iota(indices.begin(), indices.end(), 0);

    // Перемешиваем
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    // Берем первые n
    size_t count = std::min(n, static_cast<size_t>(distance));
    result.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        auto it = first;
        std::advance(it, indices[i]);
        result.emplace_back(*it);
    }

    return result;
}

// Перегрузка с std::span
std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(std::span<const Book> books, size_t n) {
    return sampleRandomBooks(books.begin(), books.end(), n);
}

// Функция для выбора топ N книг по рейтингу
// Может изменять контейнер (сортировать)
template <BookComparator Comp = comp::GreaterByRating>
std::vector<std::reference_wrapper<const Book>> getTopNBy(BookContainerLike auto &&books, size_t n, Comp comp = {}) {
    std::vector<std::reference_wrapper<const Book>> result;

    if (books.empty() || n == 0)
        return result;

    // Создаем вектор ссылок
    std::vector<std::reference_wrapper<const Book>> refs;
    refs.reserve(books.size());
    for (const auto &book : books) {
        refs.emplace_back(book);
    }

    // Сортируем по компаратору
    std::sort(refs.begin(), refs.end(), comp);

    // Берем первые n
    size_t count = std::min(n, refs.size());
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        result.emplace_back(refs[i]);
    }

    return result;
}

}  // namespace bookdb