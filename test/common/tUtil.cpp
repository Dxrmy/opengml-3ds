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

TEST_CASE( "trim functions" )
{
    SECTION("ltrim") {
        std::string s1 = "  hello";
        ltrim(s1);
        REQUIRE(s1 == "hello");

        std::string s2 = "hello  ";
        ltrim(s2);
        REQUIRE(s2 == "hello  ");

        std::string s3 = " \t\n text \r ";
        ltrim(s3);
        REQUIRE(s3 == "text \r ");

        std::string s4 = "   ";
        ltrim(s4);
        REQUIRE(s4 == "");

        std::string s5 = "";
        ltrim(s5);
        REQUIRE(s5 == "");

        std::string s6 = "no_whitespace";
        ltrim(s6);
        REQUIRE(s6 == "no_whitespace");
    }

    SECTION("rtrim") {
        std::string s1 = "  hello";
        rtrim(s1);
        REQUIRE(s1 == "  hello");

        std::string s2 = "hello  ";
        rtrim(s2);
        REQUIRE(s2 == "hello");

        std::string s3 = " \t\n text \r ";
        rtrim(s3);
        REQUIRE(s3 == " \t\n text");

        std::string s4 = "   ";
        rtrim(s4);
        REQUIRE(s4 == "");

        std::string s5 = "";
        rtrim(s5);
        REQUIRE(s5 == "");

        std::string s6 = "no_whitespace";
        rtrim(s6);
        REQUIRE(s6 == "no_whitespace");
    }

    SECTION("trim") {
        std::string s1 = "  hello  ";
        trim(s1);
        REQUIRE(s1 == "hello");

        std::string s2 = " \t\n text \r ";
        trim(s2);
        REQUIRE(s2 == "text");

        std::string s3 = "   ";
        trim(s3);
        REQUIRE(s3 == "");

        std::string s4 = "";
        trim(s4);
        REQUIRE(s4 == "");

        std::string s5 = "no_whitespace";
        trim(s5);
        REQUIRE(s5 == "no_whitespace");
    }

    SECTION("ext_trim") {
        REQUIRE(ext_trim("  hello  ") == "hello");
        REQUIRE(ext_trim(" \t\n text \r ") == "text");
        REQUIRE(ext_trim("   ") == "");
        REQUIRE(ext_trim("") == "");
        REQUIRE(ext_trim("no_whitespace") == "no_whitespace");
    }

    SECTION("ext_rtrim") {
        REQUIRE(ext_rtrim("  hello  ") == "  hello");
        REQUIRE(ext_rtrim(" \t\n text \r ") == " \t\n text");
        REQUIRE(ext_rtrim("   ") == "");
        REQUIRE(ext_rtrim("") == "");
        REQUIRE(ext_rtrim("no_whitespace") == "no_whitespace");
    }
}
