#pragma once

#include <flat_map>
#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

// Ваш код для constexpr преобразования строк в enum::Genre и наоборот здесь

constexpr Genre GenreFromString(std::string_view s) {
    if (s == "Fiction")
        return Genre::Fiction;
    if (s == "NonFiction")
        return Genre::NonFiction;
    if (s == "SciFi")
        return Genre::SciFi;
    if (s == "Biography")
        return Genre::Biography;
    if (s == "Mystery")
        return Genre::Mystery;
    return Genre::Unknown;
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
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
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
