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

TEST_CASE( "split" )
{
    std::vector<std::string> out;

    SECTION( "default split (space, combine=true)" )
    {
        split(out, "  a  b   c ");
        REQUIRE(out.size() == 3);
        REQUIRE(out[0] == "a");
        REQUIRE(out[1] == "b");
        REQUIRE(out[2] == "c");
    }

    SECTION( "split (space, combine=false)" )
    {
        out.clear();
        split(out, " a  b ", " ", false);
        // expected: "", "a", "", "b", ""
        REQUIRE(out.size() == 5);
        if (out.size() >= 5)
        {
            REQUIRE(out[0] == "");
            REQUIRE(out[1] == "a");
            REQUIRE(out[2] == "");
            REQUIRE(out[3] == "b");
            REQUIRE(out[4] == "");
        }
    }

    SECTION( "multi-character delimiter" )
    {
        out.clear();
        split(out, "abc--def--", "--", true);
        REQUIRE(out.size() == 2);
        REQUIRE(out[0] == "abc");
        REQUIRE(out[1] == "def");

        out.clear();
        split(out, "abc--def--", "--", false);
        // expected: "abc", "def", ""
        REQUIRE(out.size() == 3);
        if (out.size() >= 3)
        {
            REQUIRE(out[0] == "abc");
            REQUIRE(out[1] == "def");
            REQUIRE(out[2] == "");
        }
    }

    SECTION( "empty string" )
    {
        out.clear();
        split(out, "", " ", true);
        REQUIRE(out.size() == 0);

        out.clear();
        split(out, "", " ", false);
        REQUIRE(out.size() == 1);
        if (out.size() >= 1)
        {
            REQUIRE(out[0] == "");
        }
    }

    SECTION( "only delimiters" )
    {
        out.clear();
        split(out, "   ", " ", true);
        REQUIRE(out.size() == 0);

        out.clear();
        split(out, "   ", " ", false);
        // " ", " ", " " -> "", "", "", ""
        REQUIRE(out.size() == 4);
    }
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
