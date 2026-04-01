#define CATCH_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_MAIN
#include "catch/catch.hpp"

TEST_CASE( "Catch works", "[main]" )
{
    REQUIRE(true);
}
