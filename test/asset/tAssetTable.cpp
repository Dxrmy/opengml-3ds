#include "catch/catch.hpp"
#include "ogm/asset/AssetTable.hpp"

using namespace ogm::asset;

TEST_CASE("AssetTable basic operations", "[AssetTable]") {
    AssetTable table;

    SECTION("Empty table") {
        REQUIRE(table.asset_count() == 0);
        Asset* null_asset_by_id = table.get_asset<Asset*>(0);
        REQUIRE(!null_asset_by_id);
        Asset* missing_asset = table.get_asset("non_existent");
        REQUIRE(!missing_asset);
        const char* null_name = nullptr;
        ogm::asset_index_t index;
        Asset* null_asset_by_name = table.get_asset(null_name);
        REQUIRE(!null_asset_by_name);
        Asset* null_asset_by_name_and_index = table.get_asset(null_name, index);
        REQUIRE(!null_asset_by_name_and_index);
        Asset* empty_asset = table.get_asset("");
        REQUIRE(!empty_asset);
    }

    SECTION("Add and get asset") {
        ogm::asset_index_t index;
        Asset* new_asset = table.add_asset<Asset>("test_asset", &index);

        REQUIRE(new_asset);
        REQUIRE(index == 0);
        REQUIRE(table.asset_count() == 1);

        Asset* retrieved_by_index = table.get_asset<Asset*>(0);
        REQUIRE(retrieved_by_index == new_asset);

        Asset* retrieved_by_name = table.get_asset("test_asset");
        REQUIRE(retrieved_by_name == new_asset);

        ogm::asset_index_t out_index;
        Asset* retrieved_by_name_and_index = table.get_asset("test_asset", out_index);
        REQUIRE(retrieved_by_name_and_index == new_asset);
        REQUIRE(out_index == 0);

        const char* name = table.get_asset_name(0);
        REQUIRE(name != nullptr);
        REQUIRE(std::string(name) == "test_asset");
    }

    SECTION("Clear table") {
        table.add_asset<Asset>("test_asset1");
        table.add_asset<Asset>("test_asset2");
        REQUIRE(table.asset_count() == 2);

        table.clear();
        REQUIRE(table.asset_count() == 0);
        Asset* null_asset_after_clear = table.get_asset<Asset*>(0);
        REQUIRE(!null_asset_after_clear);
        Asset* named_asset_after_clear = table.get_asset("test_asset1");
        REQUIRE(!named_asset_after_clear);
    }
}
