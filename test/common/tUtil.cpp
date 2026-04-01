#include "catch/catch.hpp"
#include "ogm/common/util.hpp"

#include <iostream>

using namespace ogm;

TEST_CASE( "ends_with" )
{
    REQUIRE(ends_with("b/a.gml", ".gml"));
    REQUIRE(!ends_with("b/a.gml", ".ogm"));
    REQUIRE(!ends_with("b/a.gml", "."));
    REQUIRE(!ends_with("b/a.gml", ".."));
}

TEST_CASE( "replace_all" )
{
    // Normal case
    REQUIRE(replace_all("hello world", "world", "universe") == "hello universe");

    // Empty 'from' string
    REQUIRE(replace_all("hello world", "", "universe") == "hello world");

    // No matches
    REQUIRE(replace_all("hello world", "foo", "bar") == "hello world");

    // Consecutive matches
    REQUIRE(replace_all("aaaa", "aa", "b") == "bb");

    // Replacement string contains the search string (no infinite recursion)
    REQUIRE(replace_all("hello foo world", "foo", "foobar") == "hello foobar world");

    // Empty 'to' string
    REQUIRE(replace_all("hello world", "world", "") == "hello ");

    // Multiple disconnected matches
    REQUIRE(replace_all("a b a c a", "a", "x") == "x b x c x");
}
