#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"
#include "ogm/asset/Asset.hpp"

using namespace ogm::asset;
using namespace ogm;

struct DummyAsset : public Asset
{
    DummyAsset() : Asset() {}
    ~DummyAsset() override {}
    int test_value = 42;
};

TEST_CASE( "AssetTable::add_asset", "[AssetTable]" )
{
    AssetTable table;

    SECTION("Basic adding increases count and returns valid pointer")
    {
        REQUIRE(table.asset_count() == 0);

        DummyAsset* asset = table.add_asset<DummyAsset>("my_asset");
        REQUIRE(asset != nullptr);
        REQUIRE(asset->test_value == 42);
        REQUIRE(table.asset_count() == 1);

        DummyAsset* asset2 = table.add_asset<DummyAsset>("my_asset2");
        REQUIRE(asset2 != nullptr);
        REQUIRE(table.asset_count() == 2);
    }

    SECTION("add_asset sets outAssetIndex correctly")
    {
        asset_index_t index1;
        DummyAsset* asset1 = table.add_asset<DummyAsset>("asset1", &index1);
        REQUIRE(asset1 != nullptr);
        REQUIRE(index1 == 0);

        asset_index_t index2;
        DummyAsset* asset2 = table.add_asset<DummyAsset>("asset2", &index2);
        REQUIRE(asset2 != nullptr);
        REQUIRE(index2 == 1);
    }

    SECTION("Added assets can be retrieved")
    {
        asset_index_t index1;
        DummyAsset* asset1 = table.add_asset<DummyAsset>("test_asset", &index1);

        // Retrieve by name
        Asset* retrieved_by_name = table.get_asset("test_asset");
        REQUIRE(retrieved_by_name == asset1);

        // Retrieve by name and index output
        asset_index_t out_index;
        Asset* retrieved_by_name_out = table.get_asset("test_asset", out_index);
        REQUIRE(retrieved_by_name_out == asset1);
        REQUIRE(out_index == index1);

        // Retrieve by index
        Asset* retrieved_by_index = table.get_asset(index1);
        REQUIRE(retrieved_by_index == asset1);

        // Retrieve name
        const char* name = table.get_asset_name(index1);
        REQUIRE(name != nullptr);
        REQUIRE(std::string(name) == "test_asset");
    }
}
