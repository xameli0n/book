#include <gtest/gtest.h>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statistics.hpp"

using namespace bookdb;

TEST(BookDatabaseTest, AddBooksAndCheckSize) {
    BookDatabase<> db;

    Book book1{"1984", "George Orwell", 1949, Genre::Fiction, 4.5, 100};
    Book book2{"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.2, 80};

    db.PushBack(book1);
    db.push_back(book2);

    EXPECT_EQ(db.size(), 2);
    EXPECT_FALSE(db.empty());

    EXPECT_EQ(db.GetBooks()[0].author, "1984");
    EXPECT_EQ(db.GetBooks()[0].title, "George Orwell");
    EXPECT_EQ(db.GetBooks()[1].author, "Animal Farm");
    EXPECT_EQ(db.GetBooks()[1].title, "George Orwell");
}

TEST(BookDatabaseTest, InitializerListConstructor) {
    Book book1{"1984", "George Orwell", 1949, Genre::Fiction, 4.5, 100};
    Book book2{"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.2, 80};

    BookDatabase<> db{book1, book2};

    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetBooks()[0].author, "1984");
    EXPECT_EQ(db.GetBooks()[0].title, "George Orwell");
    EXPECT_EQ(db.GetBooks()[1].author, "Animal Farm");
    EXPECT_EQ(db.GetBooks()[1].title, "George Orwell");
}

TEST(BookDatabaseTest, AuthorsIndex) {
    BookDatabase<> db;

    db.EmplaceBack("George Orwell", "1984", 1949, Genre::Fiction, 4.5, 100);
    db.EmplaceBack("George Orwell", "Animal Farm", 1945, Genre::Fiction, 4.2, 80);
    db.EmplaceBack("Jane Austen", "Pride and Prejudice", 1813, Genre::Fiction, 4.7, 120);

    EXPECT_EQ(db.GetAuthors().size(), 2);  // Orwell и Austen

    EXPECT_TRUE(db.GetAuthors().count("George Orwell"));
    EXPECT_TRUE(db.GetAuthors().count("Jane Austen"));
    EXPECT_FALSE(db.GetAuthors().count("Unknown Author"));
}

TEST(BookDatabaseTest, Iterators) {
    BookDatabase<> db;

    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 4.0, 50);
    db.EmplaceBack("Author2", "Book2", 2001, Genre::NonFiction, 4.5, 60);

    auto it = db.begin();
    EXPECT_EQ(it->title, "Book1");
    ++it;
    EXPECT_EQ(it->title, "Book2");
    ++it;
    EXPECT_EQ(it, db.end());

    int count = 0;
    for (const auto &book : db) {
        EXPECT_TRUE(book.title == "Book1" || book.title == "Book2");
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST(FiltersTest, YearBetween) {
    Book book{"Author", "Title", 1950, Genre::Fiction, 4.0, 100};

    auto pred = YearBetween(1940, 1960);
    EXPECT_TRUE(pred(book));

    auto pred2 = YearBetween(1960, 1970);
    EXPECT_FALSE(pred2(book));
}

TEST(FiltersTest, RatingAbove) {
    Book book{"Author", "Title", 1950, Genre::Fiction, 4.5, 100};

    auto pred = RatingAbove(4.0);
    EXPECT_TRUE(pred(book));

    auto pred2 = RatingAbove(5.0);
    EXPECT_FALSE(pred2(book));
}

TEST(FiltersTest, GenreIs) {
    Book book{"Author", "Title", 1950, Genre::Fiction, 4.5, 100};

    auto pred = GenreIs(Genre::Fiction);
    EXPECT_TRUE(pred(book));

    auto pred2 = GenreIs(Genre::SciFi);
    EXPECT_FALSE(pred2(book));
}

TEST(FiltersTest, AllOf) {
    Book book{"Author", "Title", 1950, Genre::Fiction, 4.5, 100};

    auto pred = all_of(YearBetween(1940, 1960), RatingAbove(4.0), GenreIs(Genre::Fiction));
    EXPECT_TRUE(pred(book));

    auto pred2 = all_of(YearBetween(1960, 1970), RatingAbove(4.0));
    EXPECT_FALSE(pred2(book));
}

TEST(FiltersTest, AnyOf) {
    Book book{"Author", "Title", 1950, Genre::Fiction, 4.5, 100};

    auto pred = any_of(YearBetween(1940, 1960), RatingAbove(5.0));
    EXPECT_TRUE(pred(book));

    auto pred2 = any_of(YearBetween(1960, 1970), RatingAbove(5.0));
    EXPECT_FALSE(pred2(book));
}

TEST(FiltersTest, FilterBooks) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 1950, Genre::Fiction, 4.5, 100);
    db.EmplaceBack("Author2", "Book2", 1960, Genre::SciFi, 3.5, 80);
    db.EmplaceBack("Author3", "Book3", 1940, Genre::Fiction, 4.8, 120);

    auto filtered = filterBooks(db.begin(), db.end(), RatingAbove(4.0));
    EXPECT_EQ(filtered.size(), 2);

    EXPECT_EQ(filtered[0].get().title, "Book1");
    EXPECT_EQ(filtered[1].get().title, "Book3");
}

TEST(StatisticsTest, BuildAuthorHistogramFlat_Empty) {
    BookDatabase<> db;
    auto histogram = buildAuthorHistogramFlat(db);
    EXPECT_TRUE(histogram.empty());
}

TEST(StatisticsTest, BuildAuthorHistogramFlat) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 4.0, 50);
    db.EmplaceBack("Author1", "Book2", 2001, Genre::Fiction, 4.5, 60);
    db.EmplaceBack("Author2", "Book3", 2002, Genre::SciFi, 3.5, 40);

    auto histogram = buildAuthorHistogramFlat(db);
    EXPECT_EQ(histogram.size(), 2);
    EXPECT_EQ(histogram["Author1"], 2);
    EXPECT_EQ(histogram["Author2"], 1);
}

TEST(StatisticsTest, CalculateGenreRatings_Empty) {
    BookDatabase<> db;
    auto ratings = calculateGenreRatings(db.begin(), db.end());
    EXPECT_TRUE(ratings.empty());
}

TEST(StatisticsTest, CalculateGenreRatings) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 4.0, 50);
    db.EmplaceBack("Author2", "Book2", 2001, Genre::Fiction, 5.0, 60);
    db.EmplaceBack("Author3", "Book3", 2002, Genre::SciFi, 3.0, 40);

    auto ratings = calculateGenreRatings(db.begin(), db.end());
    EXPECT_EQ(ratings.size(), 2);
    EXPECT_DOUBLE_EQ(ratings[Genre::Fiction], 4.5);  // (4.0 + 5.0) / 2
    EXPECT_DOUBLE_EQ(ratings[Genre::SciFi], 3.0);
}

TEST(StatisticsTest, CalculateAverageRating_Empty) {
    BookDatabase<> db;
    double avg = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(avg, 0.0);
}

TEST(StatisticsTest, CalculateAverageRating) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 4.0, 50);
    db.EmplaceBack("Author2", "Book2", 2001, Genre::Fiction, 5.0, 60);

    double avg = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(avg, 4.5);
}

TEST(StatisticsTest, SampleRandomBooks_Empty) {
    BookDatabase<> db;
    auto sampled = sampleRandomBooks(db.begin(), db.end(), 5);
    EXPECT_TRUE(sampled.empty());
}

TEST(StatisticsTest, SampleRandomBooks) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 4.0, 50);
    db.EmplaceBack("Author2", "Book2", 2001, Genre::Fiction, 5.0, 60);
    db.EmplaceBack("Author3", "Book3", 2002, Genre::SciFi, 3.0, 40);

    auto sampled = sampleRandomBooks(db.begin(), db.end(), 2);
    EXPECT_EQ(sampled.size(), 2);
    // Проверяем что все выбранные книги действительно из базы
    for (const auto &ref : sampled) {
        bool found = false;
        for (const auto &book : db) {
            if (&ref.get() == &book) {
                found = true;
                break;
            }
        }
        EXPECT_TRUE(found);
    }
}

TEST(StatisticsTest, GetTopNBy_Empty) {
    BookDatabase<> db;
    auto top = getTopNBy(db, 3);
    EXPECT_TRUE(top.empty());
}

TEST(StatisticsTest, GetTopNBy) {
    BookDatabase<> db;
    db.EmplaceBack("Author1", "Book1", 2000, Genre::Fiction, 3.0, 50);
    db.EmplaceBack("Author2", "Book2", 2001, Genre::Fiction, 5.0, 60);
    db.EmplaceBack("Author3", "Book3", 2002, Genre::SciFi, 4.0, 40);

    auto top = getTopNBy(db, 2);
    EXPECT_EQ(top.size(), 2);
    EXPECT_DOUBLE_EQ(top[0].get().rating, 5.0);
    EXPECT_DOUBLE_EQ(top[1].get().rating, 4.0);
}

TEST(BookDatabaseTest, EdgeCases) {
    BookDatabase<> db;

    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.GetAuthors().empty());

    db.EmplaceBack("Author", "Title", 2000, Genre::Fiction, 4.0, 100);
    EXPECT_FALSE(db.empty());
    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetAuthors().size(), 1);

    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST(ComparatorsTest, LessByRating) {
    Book b1{"A", "T1", 2000, Genre::Fiction, 3.0, 50};
    Book b2{"A", "T2", 2000, Genre::Fiction, 4.0, 50};

    comp::LessByRating comp;
    EXPECT_TRUE(comp(b1, b2));  // 3.0 < 4.0
    EXPECT_FALSE(comp(b2, b1));
}

TEST(ComparatorsTest, LessByAuthor) {
    Book b1{"AuthorA", "Title", 2000, Genre::Fiction, 4.0, 50};
    Book b2{"AuthorB", "Title", 2000, Genre::Fiction, 4.0, 50};

    comp::LessByAuthor comp;
    EXPECT_TRUE(comp(b1, b2));
    EXPECT_FALSE(comp(b2, b1));
}
