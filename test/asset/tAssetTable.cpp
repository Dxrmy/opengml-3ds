#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"

using namespace ogm::asset;

TEST_CASE( "AssetTable clear", "[AssetTable]" )
{
    AssetTable table;

    // Initially empty
    REQUIRE(table.asset_count() == 0);

    // Add some assets
    ogm::asset_index_t index1;
    Asset* asset1 = table.add_asset<Asset>("asset1", &index1);

    ogm::asset_index_t index2;
    Asset* asset2 = table.add_asset<Asset>("asset2", &index2);

    // Verify added
    REQUIRE(table.asset_count() == 2);
    REQUIRE(table.get_asset("asset1") == asset1);
    REQUIRE(table.get_asset("asset2") == asset2);
    REQUIRE(table.get_asset(index1) == asset1);
    REQUIRE(table.get_asset(index2) == asset2);

    // Clear the table
    table.clear();

    // Verify state is clean
    REQUIRE(table.asset_count() == 0);
    REQUIRE(table.get_asset("asset1") == nullptr);
    REQUIRE(table.get_asset("asset2") == nullptr);
    REQUIRE(table.get_asset(index1) == nullptr);
    REQUIRE(table.get_asset(index2) == nullptr);
}
