#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <forward_list>
#include <ra/sv_set.hpp>

TEMPLATE_TEST_CASE("Default constructor", "[sv_set]", int, double, char) {
    using sv_set_t = ra::container::sv_set<TestType>;

    sv_set_t sv_set;

    CHECK(sv_set.size() == 0);
    CHECK(sv_set.capacity() == 0);
    CHECK(sv_set.begin() == sv_set.end());
    CHECK(sv_set.key_comp()(1, 2) == true);
}

TEMPLATE_TEST_CASE("Construct a set with the specified comparison object", "[sv_set]", int, double, char) {
    using sv_set_t = ra::container::sv_set<TestType>;

    sv_set_t sv_set(std::less<TestType>{});

    CHECK(sv_set.size() == 0);
    CHECK(sv_set.capacity() == 0);
    CHECK(sv_set.begin() == sv_set.end());
    CHECK(sv_set.key_comp()(1, 2) == true);
}

TEMPLATE_TEST_CASE("Construct a set from a range", "[sv_set]", int, double) {
    SECTION("Less than") {
        using sv_set_t = ra::container::sv_set<TestType, std::less<TestType>>;

        TestType arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 10);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
    }

    SECTION("Greater than") {
        using sv_set_t = ra::container::sv_set<TestType, std::greater<TestType>>;

        TestType arr[] = {10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 10);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == false);
    }
}

TEMPLATE_TEST_CASE("Move construct a set & Move assign a set", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5), TestType(6), TestType(7), TestType(8), TestType(9), TestType(10)};
    sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

    SECTION("Move construct") {
        sv_set_t sv_set2(std::move(sv_set));

        CHECK(sv_set.size() == 0);
        CHECK(sv_set.capacity() == 0);
        CHECK(sv_set.begin() == sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);

        CHECK(sv_set2.size() == 10);
        CHECK(sv_set2.capacity() == 10);
        CHECK(sv_set2.begin() != sv_set2.end());
        CHECK(sv_set2.key_comp()(1, 2) == true);
    }

    SECTION("Move assign") {
        sv_set_t sv_set2;
        sv_set2 = std::move(sv_set);

        CHECK(sv_set.size() == 0);
        CHECK(sv_set.capacity() == 0);
        CHECK(sv_set.begin() == sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);

        CHECK(sv_set2.size() == 10);
        CHECK(sv_set2.capacity() == 10);
        CHECK(sv_set2.begin() != sv_set2.end());
        CHECK(sv_set2.key_comp()(1, 2) == true);
    }
}

TEMPLATE_TEST_CASE("Copy construct a set & Copy assign a set", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5), TestType(6), TestType(7), TestType(8), TestType(9), TestType(10)};
    sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

    SECTION("Copy construct") {
        sv_set_t sv_set2(sv_set);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 10);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);

        CHECK(sv_set2.size() == 10);
        CHECK(sv_set2.capacity() == 10);
        CHECK(sv_set2.begin() != sv_set2.end());
        CHECK(sv_set2.key_comp()(1, 2) == true);
    }

    SECTION("Copy assign") {
        sv_set_t sv_set2;
        sv_set2 = sv_set;

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 10);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);

        CHECK(sv_set2.size() == 10);
        CHECK(sv_set2.capacity() == 10);
        CHECK(sv_set2.begin() != sv_set2.end());
        CHECK(sv_set2.key_comp()(1, 2) == true);
    }
}

TEMPLATE_TEST_CASE("Insert and erase", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5), TestType(7), TestType(8), TestType(9), TestType(10)};
    sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

    SECTION("Insert to an empty set") {
        sv_set_t sv_set2;
        sv_set2.insert(TestType(6));

        CHECK(sv_set2.size() == 1);
        CHECK(sv_set2.capacity() == 2);
        CHECK(sv_set2.begin() != sv_set2.end());
        CHECK(sv_set2.key_comp()(1, 2) == true);
    }

    SECTION("Insert to the begin, reallocated") {
        auto result = sv_set.insert(TestType(0));

        CHECK(result.first == sv_set.begin());
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 18);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(0));
        CHECK(*(sv_set.begin() + 1) == TestType(1));
        CHECK(*(sv_set.begin() + 2) == TestType(2));
        CHECK(*(sv_set.begin() + 3) == TestType(3));
        CHECK(*(sv_set.begin() + 4) == TestType(4));
        CHECK(*(sv_set.begin() + 5) == TestType(5));
        CHECK(*(sv_set.begin() + 6) == TestType(7));
        CHECK(*(sv_set.begin() + 7) == TestType(8));
        CHECK(*(sv_set.begin() + 8) == TestType(9));
        CHECK(*(sv_set.begin() + 9) == TestType(10));
    }

    SECTION("Insert to the middle, reallocated") {
        auto result = sv_set.insert(TestType(6));

        CHECK(result.first == sv_set.begin() + 5);
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 18);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(6));
        CHECK(*(sv_set.begin() + 6) == TestType(7));
        CHECK(*(sv_set.begin() + 7) == TestType(8));
        CHECK(*(sv_set.begin() + 8) == TestType(9));
        CHECK(*(sv_set.begin() + 9) == TestType(10));
    }

    SECTION("Insert to the end, reallocated") {
        auto result = sv_set.insert(TestType(11));

        CHECK(result.first == sv_set.end() - 1);
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 18);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(7));
        CHECK(*(sv_set.begin() + 6) == TestType(8));
        CHECK(*(sv_set.begin() + 7) == TestType(9));
        CHECK(*(sv_set.begin() + 8) == TestType(10));
        CHECK(*(sv_set.begin() + 9) == TestType(11));
    }

    SECTION("Insert with existing key, size == capacity") {
        auto result = sv_set.insert(TestType(5));

        CHECK(result.first == sv_set.begin() + 4);
        CHECK(result.second == false);

        CHECK(sv_set.size() == 9);
        CHECK(sv_set.capacity() == 9);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(7));
        CHECK(*(sv_set.begin() + 6) == TestType(8));
        CHECK(*(sv_set.begin() + 7) == TestType(9));
        CHECK(*(sv_set.begin() + 8) == TestType(10));
    }

    SECTION("Insert to the begin, no reallocated") {
        sv_set.reserve(20);

        auto result = sv_set.insert(TestType(0));

        CHECK(result.first == sv_set.begin());
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 20);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(0));
        CHECK(*(sv_set.begin() + 1) == TestType(1));
        CHECK(*(sv_set.begin() + 2) == TestType(2));
        CHECK(*(sv_set.begin() + 3) == TestType(3));
        CHECK(*(sv_set.begin() + 4) == TestType(4));
        CHECK(*(sv_set.begin() + 5) == TestType(5));
        CHECK(*(sv_set.begin() + 6) == TestType(7));
        CHECK(*(sv_set.begin() + 7) == TestType(8));
        CHECK(*(sv_set.begin() + 8) == TestType(9));
        CHECK(*(sv_set.begin() + 9) == TestType(10));
    }

    SECTION("Insert to the middle, no reallocated"){
        sv_set.reserve(20);

        auto result = sv_set.insert(TestType(6));
        
        CHECK(result.first == sv_set.begin() + 5);
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 20);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(6));
        CHECK(*(sv_set.begin() + 6) == TestType(7));
        CHECK(*(sv_set.begin() + 7) == TestType(8));
        CHECK(*(sv_set.begin() + 8) == TestType(9));
        CHECK(*(sv_set.begin() + 9) == TestType(10));
    }

    SECTION("Insert to the end, no reallocated"){
        sv_set.reserve(20);

        auto result = sv_set.insert(TestType(11));
        
        CHECK(result.first == sv_set.end() - 1);
        CHECK(result.second == true);

        CHECK(sv_set.size() == 10);
        CHECK(sv_set.capacity() == 20);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(7));
        CHECK(*(sv_set.begin() + 6) == TestType(8));
        CHECK(*(sv_set.begin() + 7) == TestType(9));
        CHECK(*(sv_set.begin() + 8) == TestType(10));
        CHECK(*(sv_set.begin() + 9) == TestType(11));
    }

    SECTION("Insert with existing key, size != capacity") {
        sv_set.reserve(20);
        sv_set.insert(TestType(5));
        
        auto result = sv_set.insert(TestType(5));
        
        CHECK(result.first == sv_set.begin() + 4);
        CHECK(result.second == false);
        
        CHECK(sv_set.size() == 9);
        CHECK(sv_set.capacity() == 20);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(7));
        CHECK(*(sv_set.begin() + 6) == TestType(8));
        CHECK(*(sv_set.begin() + 7) == TestType(9));
        CHECK(*(sv_set.begin() + 8) == TestType(10));
    }

    SECTION("Erase from the middle") {
        CHECK(*(sv_set.erase(sv_set.begin() + 3)) == TestType(5));

        CHECK(sv_set.size() == 8);
        CHECK(sv_set.capacity() == 9);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(5));
        CHECK(*(sv_set.begin() + 4) == TestType(7));
        CHECK(*(sv_set.begin() + 5) == TestType(8));
        CHECK(*(sv_set.begin() + 6) == TestType(9));
        CHECK(*(sv_set.begin() + 7) == TestType(10));
    }

    SECTION("Erase from the end") {
        CHECK((sv_set.erase(sv_set.begin() + 8) == sv_set.end()));

        CHECK(sv_set.size() == 8);
        CHECK(sv_set.capacity() == 9);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));
        CHECK(*(sv_set.begin() + 5) == TestType(7));
        CHECK(*(sv_set.begin() + 6) == TestType(8));
        CHECK(*(sv_set.begin() + 7) == TestType(9));
    }
}

TEMPLATE_TEST_CASE("Reserve and shrink_to_fit", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5), TestType(6), TestType(7), TestType(8), TestType(9), TestType(10)};
    sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

    sv_set.reserve(20);

    CHECK(sv_set.size() == 10);
    CHECK(sv_set.capacity() == 20);
    CHECK(sv_set.begin() != sv_set.end());
    CHECK(sv_set.key_comp()(1, 2) == true);
    CHECK(*sv_set.begin() == TestType(1));
    CHECK(*(sv_set.begin() + 1) == TestType(2));
    CHECK(*(sv_set.begin() + 2) == TestType(3));
    CHECK(*(sv_set.begin() + 3) == TestType(4));
    CHECK(*(sv_set.begin() + 4) == TestType(5));
    CHECK(*(sv_set.begin() + 5) == TestType(6));
    CHECK(*(sv_set.begin() + 6) == TestType(7));
    CHECK(*(sv_set.begin() + 7) == TestType(8));
    CHECK(*(sv_set.begin() + 8) == TestType(9));

    sv_set.shrink_to_fit();

    CHECK(sv_set.size() == 10);
    CHECK(sv_set.capacity() == 10);
    CHECK(sv_set.begin() != sv_set.end());
    CHECK(sv_set.key_comp()(1, 2) == true);
    CHECK(*sv_set.begin() == TestType(1));
    CHECK(*(sv_set.begin() + 1) == TestType(2));
    CHECK(*(sv_set.begin() + 2) == TestType(3));
    CHECK(*(sv_set.begin() + 3) == TestType(4));
    CHECK(*(sv_set.begin() + 4) == TestType(5));
    CHECK(*(sv_set.begin() + 5) == TestType(6));
    CHECK(*(sv_set.begin() + 6) == TestType(7));
    CHECK(*(sv_set.begin() + 7) == TestType(8));
    CHECK(*(sv_set.begin() + 8) == TestType(9));
}

TEMPLATE_TEST_CASE("Swap the contents of two sets", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5)};
    TestType arr2[] = {TestType(6), TestType(7), TestType(8), TestType(9), TestType(10)};
    sv_set_t sv_set1(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));
    sv_set_t sv_set2(typename sv_set_t::ordered_and_unique_range(), std::begin(arr2), std::end(arr2) - std::begin(arr2));

    CHECK(sv_set1.size() == 5);
    CHECK(sv_set1.capacity() == 5);
    CHECK(sv_set1.begin() != sv_set1.end());
    CHECK(sv_set1.key_comp()(1, 2) == true);
    CHECK(*sv_set1.begin() == TestType(1));
    CHECK(*(sv_set1.begin() + 1) == TestType(2));
    CHECK(*(sv_set1.begin() + 2) == TestType(3));
    CHECK(*(sv_set1.begin() + 3) == TestType(4));
    CHECK(*(sv_set1.begin() + 4) == TestType(5));

    CHECK(sv_set2.size() == 5);
    CHECK(sv_set2.capacity() == 5);
    CHECK(sv_set2.begin() != sv_set2.end());
    CHECK(sv_set2.key_comp()(1, 2) == true);
    CHECK(*sv_set2.begin() == TestType(6));
    CHECK(*(sv_set2.begin() + 1) == TestType(7));
    CHECK(*(sv_set2.begin() + 2) == TestType(8));
    CHECK(*(sv_set2.begin() + 3) == TestType(9));
    CHECK(*(sv_set2.begin() + 4) == TestType(10));

    sv_set1.swap(sv_set2);

    CHECK(sv_set1.size() == 5);
    CHECK(sv_set1.capacity() == 5);
    CHECK(sv_set1.begin() != sv_set1.end());
    CHECK(sv_set1.key_comp()(1, 2) == true);
    CHECK(*sv_set1.begin() == TestType(6));
    CHECK(*(sv_set1.begin() + 1) == TestType(7));
    CHECK(*(sv_set1.begin() + 2) == TestType(8));
    CHECK(*(sv_set1.begin() + 3) == TestType(9));
    CHECK(*(sv_set1.begin() + 4) == TestType(10));

    CHECK(sv_set2.size() == 5);
    CHECK(sv_set2.capacity() == 5);
    CHECK(sv_set2.begin() != sv_set2.end());
    CHECK(sv_set2.key_comp()(1, 2) == true);
    CHECK(*sv_set2.begin() == TestType(1));
    CHECK(*(sv_set2.begin() + 1) == TestType(2));
    CHECK(*(sv_set2.begin() + 2) == TestType(3));
    CHECK(*(sv_set2.begin() + 3) == TestType(4));
    CHECK(*(sv_set2.begin() + 4) == TestType(5));
}

TEMPLATE_TEST_CASE("find and clear", "[sv_set]", int, double) {
    using sv_set_t = ra::container::sv_set<TestType>;

    TestType arr[] = {TestType(1), TestType(2), TestType(3), TestType(4), TestType(5)};
    sv_set_t sv_set(typename sv_set_t::ordered_and_unique_range(), std::begin(arr), std::end(arr) - std::begin(arr));

    SECTION("Find with existing key") {
        CHECK(sv_set.size() == 5);
        CHECK(sv_set.capacity() == 5);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));

        auto it = sv_set.find(TestType(3));
        CHECK(it != sv_set.end());
        CHECK(*it == TestType(3));
        CHECK(*(sv_set.erase(it)) == TestType(4));
    }

    SECTION("Find with non-existing key") {
        CHECK(sv_set.size() == 5);
        CHECK(sv_set.capacity() == 5);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));

        auto it = sv_set.find(TestType(6));
        CHECK(it == sv_set.end());
    }

    SECTION("Clear") {
        CHECK(sv_set.size() == 5);
        CHECK(sv_set.capacity() == 5);
        CHECK(sv_set.begin() != sv_set.end());
        CHECK(sv_set.key_comp()(1, 2) == true);
        CHECK(*sv_set.begin() == TestType(1));
        CHECK(*(sv_set.begin() + 1) == TestType(2));
        CHECK(*(sv_set.begin() + 2) == TestType(3));
        CHECK(*(sv_set.begin() + 3) == TestType(4));
        CHECK(*(sv_set.begin() + 4) == TestType(5));

        sv_set.clear();
        CHECK(sv_set.size() == 0);
        CHECK(sv_set.capacity() == 5);
        CHECK(sv_set.begin() == sv_set.end());
    }
}