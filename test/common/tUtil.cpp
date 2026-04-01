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

TEST_CASE( "ltrim" )
{
    std::string s;

    // Happy path: leading spaces
    s = "   hello";
    ltrim(s);
    REQUIRE(s == "hello");

    // Happy path: leading tabs and newlines
    s = "\t\n  world";
    ltrim(s);
    REQUIRE(s == "world");

    // Edge case: empty string
    s = "";
    ltrim(s);
    REQUIRE(s == "");

    // Edge case: string with no leading whitespace
    s = "no leading space";
    ltrim(s);
    REQUIRE(s == "no leading space");

    // Edge case: string consisting entirely of spaces
    s = "     ";
    ltrim(s);
    REQUIRE(s == "");

    // Edge case: trailing spaces should not be removed
    s = "  hello  ";
    ltrim(s);
    REQUIRE(s == "hello  ");

    // Edge case: single space
    s = " ";
    ltrim(s);
    REQUIRE(s == "");
}
