#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"
#include "ogm/asset/Asset.hpp"

namespace {
    // Lightweight dummy asset for testing purposes
    class DummyAsset : public ogm::asset::Asset {
    public:
        DummyAsset() = default;
        ~DummyAsset() override = default;
    };
}

TEST_CASE( "AssetTable clear functionality", "[AssetTable]" )
{
    ogm::asset::AssetTable table;

    SECTION( "clearing a populated AssetTable" )
    {
        // Populate
        ogm::asset_index_t index1, index2;
        table.add_asset<DummyAsset>("asset1", &index1);
        table.add_asset<DummyAsset>("asset2", &index2);

        REQUIRE( table.asset_count() == 2 );
        REQUIRE( table.get_asset<DummyAsset*>(index1) != nullptr );
        REQUIRE( table.get_asset<DummyAsset*>(index2) != nullptr );
        REQUIRE( table.get_asset("asset1") != nullptr );
        REQUIRE( table.get_asset("asset2") != nullptr );

        // Clear
        table.clear();

        // Verify state is empty
        REQUIRE( table.asset_count() == 0 );
        REQUIRE( table.get_asset<DummyAsset*>(index1) == nullptr );
        REQUIRE( table.get_asset<DummyAsset*>(index2) == nullptr );
        REQUIRE( table.get_asset("asset1") == nullptr );
        REQUIRE( table.get_asset("asset2") == nullptr );
    }

    SECTION( "clearing an already empty AssetTable" )
    {
        REQUIRE( table.asset_count() == 0 );

        // Clear
        table.clear();

        // Verify state is still empty and no errors occurred
        REQUIRE( table.asset_count() == 0 );
        REQUIRE( table.get_asset<DummyAsset*>(0) == nullptr );
        REQUIRE( table.get_asset("nonexistent") == nullptr );
    }
}
