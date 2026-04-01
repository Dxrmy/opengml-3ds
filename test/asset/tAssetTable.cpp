#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"
#include "ogm/common/types.hpp"

using namespace ogm::asset;
using namespace ogm;

TEST_CASE( "AssetTable get_asset by index", "[AssetTable]" ) {
    AssetTable table;

    SECTION( "Get non-existent asset returns nullptr" ) {
        REQUIRE( !table.get_asset<Asset*>(0) );
        REQUIRE( !table.get_asset<Asset*>(1) );
    }

    SECTION( "Add and get asset by index" ) {
        ogm::asset_index_t index;
        Asset* new_asset = table.add_asset<Asset>("test_asset", &index);

        REQUIRE( index == 0 );
        REQUIRE( new_asset != nullptr );

        Asset* retrieved_asset = table.get_asset<Asset*>(index);
        REQUIRE( retrieved_asset == new_asset );
    }

    SECTION( "Out of bounds index returns nullptr" ) {
        ogm::asset_index_t index;
        table.add_asset<Asset>("test_asset2", &index);

        REQUIRE( !table.get_asset<Asset*>(index + 1) );
    }
}
