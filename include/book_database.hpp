#pragma once

#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = Book;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;
    using size_type = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;
    using pointer = typename BookContainer::pointer;
    using const_pointer = typename BookContainer::const_pointer;
    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init) {
        for (const auto &book : init) {
            push_back(book);
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    // Standard container interface methods
    iterator begin() { return books_.begin(); }
    const_iterator begin() const { return books_.begin(); }
    const_iterator cbegin() const { return books_.cbegin(); }

    iterator end() { return books_.end(); }
    const_iterator end() const { return books_.end(); }
    const_iterator cend() const { return books_.cend(); }

    size_type size() const { return books_.size(); }
    bool empty() const { return books_.empty(); }

    reference operator[](size_type pos) { return books_[pos]; }
    const_reference operator[](size_type pos) const { return books_[pos]; }

    reference front() { return books_.front(); }
    const_reference front() const { return books_.front(); }

    reference back() { return books_.back(); }
    const_reference back() const { return books_.back(); }

    void push_back(const Book &book) {
        auto author_view = storeAuthor(book.author);
        Book copy = book;
        copy.author = author_view;
        books_.push_back(std::move(copy));
    }

    void push_back(Book &&book) {
        auto author_view = storeAuthor(book.author);
        book.author = author_view;
        books_.push_back(std::move(book));
    }

    void PushBack(const Book &book) { push_back(book); }

    void PushBack(Book &&book) { push_back(std::move(book)); }

    template <typename... Args>
    void emplace_back(Args &&...args) {
        Book temp_book(std::forward<Args>(args)...);
        auto author_view = storeAuthor(temp_book.author);
        temp_book.author = author_view;
        books_.push_back(std::move(temp_book));
    }

    template <typename... Args>
    void EmplaceBack(Args &&...args) {
        emplace_back(std::forward<Args>(args)...);
    }

    // Additional methods
    const BookContainer &GetBooks() const { return books_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

private:
    template <typename S>
    std::string_view storeAuthor(S &&s) {
        std::string key(std::forward<S>(s));
        auto [it, inserted] = authors_.insert(std::move(key));
        return *it;
    }

    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
