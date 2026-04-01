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

TEST_CASE( "String Trimming" )
{
    SECTION("ltrim")
    {
        std::string s1 = "   hello";
        ltrim(s1);
        REQUIRE(s1 == "hello");

        std::string s2 = "hello   ";
        ltrim(s2);
        REQUIRE(s2 == "hello   ");

        std::string s3 = "   hello   ";
        ltrim(s3);
        REQUIRE(s3 == "hello   ");

        std::string s4 = "\t\r\nhello";
        ltrim(s4);
        REQUIRE(s4 == "hello");

        std::string s5 = "hello";
        ltrim(s5);
        REQUIRE(s5 == "hello");

        std::string s6 = "   ";
        ltrim(s6);
        REQUIRE(s6 == "");

        std::string s7 = "";
        ltrim(s7);
        REQUIRE(s7 == "");
    }

    SECTION("rtrim")
    {
        std::string s1 = "hello   ";
        rtrim(s1);
        REQUIRE(s1 == "hello");

        std::string s2 = "   hello";
        rtrim(s2);
        REQUIRE(s2 == "   hello");

        std::string s3 = "   hello   ";
        rtrim(s3);
        REQUIRE(s3 == "   hello");

        std::string s4 = "hello\t\r\n";
        rtrim(s4);
        REQUIRE(s4 == "hello");

        std::string s5 = "hello";
        rtrim(s5);
        REQUIRE(s5 == "hello");

        std::string s6 = "   ";
        rtrim(s6);
        REQUIRE(s6 == "");

        std::string s7 = "";
        rtrim(s7);
        REQUIRE(s7 == "");
    }

    SECTION("trim")
    {
        std::string s1 = "   hello   ";
        trim(s1);
        REQUIRE(s1 == "hello");

        std::string s2 = "hello";
        trim(s2);
        REQUIRE(s2 == "hello");

        std::string s3 = "\t\r\nhello\t\r\n";
        trim(s3);
        REQUIRE(s3 == "hello");

        std::string s4 = "   ";
        trim(s4);
        REQUIRE(s4 == "");

        std::string s5 = "";
        trim(s5);
        REQUIRE(s5 == "");
    }
}
