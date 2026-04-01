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
    // Single replacement
    REQUIRE(replace_all("hello world", "world", "there") == "hello there");

    // Multiple replacements
    REQUIRE(replace_all("foo bar foo baz", "foo", "qux") == "qux bar qux baz");

    // No match
    REQUIRE(replace_all("hello world", "test", "replace") == "hello world");

    // Empty replacement
    REQUIRE(replace_all("remove some words", "some ", "") == "remove words");

    // Target string contained within replacement string
    REQUIRE(replace_all("abc", "b", "bb") == "abbc");
    REQUIRE(replace_all("x", "x", "xx") == "xx");

    // Empty input string
    REQUIRE(replace_all("", "a", "b") == "");

    // Empty search string (should return original string to avoid infinite loop)
    REQUIRE(replace_all("hello", "", "test") == "hello");
}
