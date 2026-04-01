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


#define TEMPLATE_TEST_CASE_SPLIT(TestType, TestName) \
TEST_CASE( TestName, "[util][string]" ) \
{ \
    std::vector<TestType> out; \
\
    SECTION( "Standard splitting" ) \
    { \
        split(out, "a,b,c", ",", true); \
        REQUIRE(out.size() == 3); \
        REQUIRE(out[0] == "a"); \
        REQUIRE(out[1] == "b"); \
        REQUIRE(out[2] == "c"); \
    } \
\
    SECTION( "Combining consecutive delimiters" ) \
    { \
        split(out, ",a,,b,c,", ",", true); \
        REQUIRE(out.size() == 3); \
        REQUIRE(out[0] == "a"); \
        REQUIRE(out[1] == "b"); \
        REQUIRE(out[2] == "c"); \
    } \
\
    SECTION( "Non-combining consecutive delimiters" ) \
    { \
        split(out, ",a,,b,c,", ",", false); \
        REQUIRE(out.size() == 6); \
        REQUIRE(out[0] == ""); \
        REQUIRE(out[1] == "a"); \
        REQUIRE(out[2] == ""); \
        REQUIRE(out[3] == "b"); \
        REQUIRE(out[4] == "c"); \
        REQUIRE(out[5] == ""); \
    } \
\
    SECTION( "Empty segment trailing delimiter" ) \
    { \
        split(out, "a,", ",", false); \
        REQUIRE(out.size() == 2); \
        REQUIRE(out[0] == "a"); \
        REQUIRE(out[1] == ""); \
    } \
\
    SECTION( "Single delimiter string non-combining" ) \
    { \
        split(out, ",", ",", false); \
        REQUIRE(out.size() == 2); \
        REQUIRE(out[0] == ""); \
        REQUIRE(out[1] == ""); \
    } \
\
    SECTION( "Single delimiter string combining" ) \
    { \
        split(out, ",", ",", true); \
        REQUIRE(out.size() == 0); \
    } \
\
    SECTION( "Multi-character delimiter non-combining" ) \
    { \
        split(out, "a..b..c", "..", false); \
        REQUIRE(out.size() == 3); \
        REQUIRE(out[0] == "a"); \
        REQUIRE(out[1] == "b"); \
        REQUIRE(out[2] == "c"); \
    } \
\
    SECTION( "Multi-character delimiter with empty segments" ) \
    { \
        split(out, "..a....b....", "..", false); \
        REQUIRE(out.size() == 6); \
        REQUIRE(out[0] == ""); \
        REQUIRE(out[1] == "a"); \
        REQUIRE(out[2] == ""); \
        REQUIRE(out[3] == "b"); \
        REQUIRE(out[4] == ""); \
        REQUIRE(out[5] == ""); \
    } \
\
    SECTION( "Empty delimiter string non-combining" ) \
    { \
        split(out, "abc", "", false); \
        REQUIRE(out.size() == 1); \
        REQUIRE(out[0] == "abc"); \
    } \
\
    SECTION( "Empty string and empty delimiter string non-combining" ) \
    { \
        split(out, "", "", false); \
        REQUIRE(out.size() == 1); \
        REQUIRE(out[0] == ""); \
    } \
\
    SECTION( "Empty string and empty delimiter string combining" ) \
    { \
        split(out, "", "", true); \
        REQUIRE(out.size() == 0); \
    } \
\
    SECTION( "Empty string and valid delimiter non-combining" ) \
    { \
        split(out, "", ",", false); \
        REQUIRE(out.size() == 1); \
        REQUIRE(out[0] == ""); \
    } \
\
    SECTION( "Empty string and valid delimiter combining" ) \
    { \
        split(out, "", ",", true); \
        REQUIRE(out.size() == 0); \
    } \
}

TEMPLATE_TEST_CASE_SPLIT(std::string, "split_std_string")
TEMPLATE_TEST_CASE_SPLIT(std::string_view, "split_std_string_view")
