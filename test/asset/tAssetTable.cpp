#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"
#include "ogm/asset/Asset.hpp"
#include "ogm/common/types.hpp"

#include <string>

using namespace ogm::asset;

class DummyAsset : public Asset {
public:
    DummyAsset() {}
    ~DummyAsset() override {}
};

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
    REQUIRE(!table.get_asset("asset1"));
    REQUIRE(!table.get_asset("asset2"));
    REQUIRE(!table.get_asset(index1));
    REQUIRE(!table.get_asset(index2));
}

TEST_CASE( "AssetTable add_asset", "[AssetTable]" ) {
    AssetTable table;

    SECTION("Add a single asset without outAssetIndex") {
        Asset* asset = table.add_asset<DummyAsset>("test_asset");
        REQUIRE(asset != nullptr);
        REQUIRE(table.asset_count() == 1);

        ogm::asset_index_t index;
        Asset* fetched_asset = table.get_asset("test_asset", index);
        REQUIRE(fetched_asset == asset);
        REQUIRE(index == 0);

        REQUIRE(std::string(table.get_asset_name(0)) == "test_asset");
    }

    SECTION("Add an asset with outAssetIndex") {
        ogm::asset_index_t index;
        Asset* asset = table.add_asset<DummyAsset>("test_asset_2", &index);
        REQUIRE(asset != nullptr);
        REQUIRE(table.asset_count() == 1);
        REQUIRE(index == 0);

        Asset* fetched_asset = table.get_asset("test_asset_2");
        REQUIRE(fetched_asset == asset);
    }

    SECTION("Add multiple assets") {
        table.add_asset<DummyAsset>("asset_1");
        table.add_asset<DummyAsset>("asset_2");
        table.add_asset<DummyAsset>("asset_3");

        REQUIRE(table.asset_count() == 3);

        ogm::asset_index_t index_2;
        table.get_asset("asset_2", index_2);
        REQUIRE(index_2 == 1);

        REQUIRE(std::string(table.get_asset_name(2)) == "asset_3");
    }

    SECTION("Add asset with empty name") {
        Asset* asset = table.add_asset<DummyAsset>("");
        REQUIRE(asset != nullptr);
        REQUIRE(table.asset_count() == 1);

        ogm::asset_index_t index;
        Asset* fetched_asset = table.get_asset("", index);
        REQUIRE(fetched_asset == asset);
        REQUIRE(index == 0);
        REQUIRE(std::string(table.get_asset_name(0)) == "");
    }

    SECTION("Add duplicate names overwrites index map but retains vector entry") {
        // According to current implementation:
        // m_name_index[assetName] = m_index_asset.size();
        // So a second add with the same name will overwrite the map, but add a new element to vector.
        ogm::asset_index_t index1, index2;
        Asset* asset1 = table.add_asset<DummyAsset>("duplicate", &index1);
        Asset* asset2 = table.add_asset<DummyAsset>("duplicate", &index2);

        REQUIRE(asset1 != nullptr);
        REQUIRE(asset2 != nullptr);
        REQUIRE(asset1 != asset2);
        REQUIRE(table.asset_count() == 2);

        REQUIRE(index1 == 0);
        REQUIRE(index2 == 1);

        ogm::asset_index_t fetched_index;
        Asset* fetched_asset = table.get_asset("duplicate", fetched_index);

        // The map should point to the latest addition
        REQUIRE(fetched_index == 1);
        REQUIRE(fetched_asset == asset2);

        // However, the original is still accessible by id
        REQUIRE(table.get_asset<DummyAsset*>(0) == asset1);
        REQUIRE(table.get_asset<DummyAsset*>(1) == asset2);
    }

    SECTION("High-frequency additions") {
        const int num_assets = 10000;
        for (int i = 0; i < num_assets; ++i) {
            std::string name = "stress_asset_" + std::to_string(i);
            ogm::asset_index_t outIndex;
            table.add_asset<DummyAsset>(name.c_str(), &outIndex);
            REQUIRE(outIndex == i);
        }
        REQUIRE(table.asset_count() == num_assets);

        // Randomly check some elements
        ogm::asset_index_t index_to_check = 5000;
        std::string name_to_check = "stress_asset_5000";
        ogm::asset_index_t fetched_index;
        table.get_asset(name_to_check.c_str(), fetched_index);
        REQUIRE(fetched_index == index_to_check);
        REQUIRE(std::string(table.get_asset_name(index_to_check)) == name_to_check);
    }

    table.clear();
}
