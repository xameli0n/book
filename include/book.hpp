#pragma once

#include <array>
#include <flat_map>
#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

// Соответствие Genre - строка
constexpr std::array<std::pair<Genre, std::string_view>, 6> GenreMap{{
    {Genre::Fiction, "Fiction"},
    {Genre::NonFiction, "NonFiction"},
    {Genre::SciFi, "SciFi"},
    {Genre::Biography, "Biography"},
    {Genre::Mystery, "Mystery"},
    {Genre::Unknown, "Unknown"},
}};

[[nodiscard]] constexpr Genre GenreFromString(std::string_view s) {
    for (const auto &[genre, str] : GenreMap) {
        if (s == str)
            return genre;
    }
    return Genre::Unknown;
}

[[nodiscard]] constexpr std::string_view StringFromGenre(Genre g) {
    for (const auto &[genre, str] : GenreMap) {
        if (genre == g)
            return str;
    }
    return "Unknown";
}

struct Book {
    // string_view для экономии памяти, чтобы ссылаться на оригинальную строку, хранящуюся в другом контейнере
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    // constexpr-конструктор, принимающий Genre как строку
    constexpr Book(std::string_view a, std::string t, int y, std::string_view g_str, double r, int rc)
        : author(a), title(std::move(t)), year(y), genre(GenreFromString(g_str)), rating(r), read_count(rc) {}

    // constexpr-конструктор, принимающий Genre как enum
    constexpr Book(std::string_view a, std::string t, int y, Genre g, double r, int rc)
        : author(a), title(std::move(t)), year(y), genre(g), rating(r), read_count(rc) {}
};
}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        return format_to(fc.out(), "{}", bookdb::StringFromGenre(g));
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &b, FormatContext &fc) const {
        return format_to(fc.out(), "Author: {}, Title: {}, Year: {}, Genre: {}, Rating: {}, Read Count: {}", b.author,
                         b.title, b.year, b.genre, b.rating, b.read_count);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <typename K, typename V, typename C, typename KC, typename VC>
struct formatter<std::flat_map<K, V, C, KC, VC>, char> {
    template <typename FormatContext>
    auto format(const std::flat_map<K, V, C, KC, VC> &m, FormatContext &fc) const {
        format_to(fc.out(), "{{");
        bool first = true;
        for (const auto &[k, v] : m) {
            if (!first)
                format_to(fc.out(), ", ");
            format_to(fc.out(), "{}: {}", k, v);
            first = false;
        }
        return format_to(fc.out(), "}}");
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

}  // namespace std
