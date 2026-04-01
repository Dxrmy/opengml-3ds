#define CATCH_CONFIG_NO_POSIX_SIGNALS
#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_NO_POSIX_SIGNALS
#include "catch/catch.hpp"

TEST_CASE( "Catch works", "[main]" )
{
    REQUIRE(true);
}
