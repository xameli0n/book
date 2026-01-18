#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book &lhs, const Book &rhs) const {
        if (lhs.author != rhs.author)
            return lhs.author < rhs.author;
        return lhs.title < rhs.title;
    }
};

struct LessByTitle {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.title < rhs.title; }
};

struct LessByYear {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.year < rhs.year; }
};

struct LessByRating {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.rating < rhs.rating; }
};

struct LessByPopularity {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.read_count < rhs.read_count; }
};

struct LessByGenre {
    bool operator()(const Book &lhs, const Book &rhs) const {
        return static_cast<int>(lhs.genre) < static_cast<int>(rhs.genre);
    }
};

struct GreaterByRating {
    bool operator()(const Book &lhs, const Book &rhs) const { return lhs.rating > rhs.rating; }
};

}  // namespace bookdb::comp