#include "catch/catch.hpp"
#include "ogm/collision/SpatialHash.hpp"
#include "ogm/geometry/aabb.hpp"
#include "ogm/common/types.hpp"

#include <vector>
#include <set>
#include <iostream>

using namespace ogm;
using namespace ogm::collision;
using namespace ogm::geometry;

typedef SpatialHash<double, 1009> SpatialHashT;
typedef SpatialHashT::aabb_t aabb_t;

TEST_CASE("SpatialHash basic operations", "[collision]")
{
    SpatialHashT hash;

    SECTION("Insert and find")
    {
        hash.insert(1, aabb_t(10.0, 10.0, 20.0, 20.0));
        hash.insert(2, aabb_t(15.0, 15.0, 25.0, 25.0));
        hash.insert(3, aabb_t(100.0, 100.0, 110.0, 110.0));

        std::set<SpatialHashT::leaf_index_t> found;
        auto insert_to_found = [&found](SpatialHashT::leaf_index_t leaf) -> bool {
            found.insert(leaf);
            return true;
        };

        // Search in a region that overlaps 1 and 2
        hash.foreach_leaf_in_aabb(aabb_t(12.0, 12.0, 16.0, 16.0), insert_to_found);

        // Due to bucket size (96x96), leaves may exist in nodes covering regions larger than their specific AABB.
        // Therefore foreach_leaf_in_aabb will return ALL leaves in nodes that overlap the queried AABB.
        // For (12,12,16,16), this overlaps bucket (0,0) [0..96, 0..96].
        // Leaves 1 and 2 are both in bucket (0,0).
        // Leaf 3 is in bucket (1,1) [96..192, 96..192]
        REQUIRE(found.count(1) == 1);
        REQUIRE(found.count(2) == 1);
        REQUIRE(found.count(3) == 0);

        found.clear();

        // Search in a region that overlaps only 3
        hash.foreach_leaf_in_aabb(aabb_t(90.0, 90.0, 105.0, 105.0), insert_to_found);
        // Overlaps buckets (0,0), (1,0), (0,1), (1,1).
        // Bucket (0,0) has 1 and 2. Bucket (1,1) has 3.
        REQUIRE(found.count(1) == 1);
        REQUIRE(found.count(2) == 1);
        REQUIRE(found.count(3) == 1);

        found.clear();

        // Search in a region that overlaps nothing
        hash.foreach_leaf_in_aabb(aabb_t(200.0, 200.0, 210.0, 210.0), insert_to_found);
        // Overlaps bucket (2,2)
        REQUIRE(found.empty());
    }

    SECTION("Remove")
    {
        hash.insert(1, aabb_t(10.0, 10.0, 20.0, 20.0));
        hash.insert(2, aabb_t(15.0, 15.0, 25.0, 25.0));

        std::set<SpatialHashT::leaf_index_t> found;
        auto insert_to_found = [&found](SpatialHashT::leaf_index_t leaf) -> bool {
            found.insert(leaf);
            return true;
        };

        hash.remove(1);

        hash.foreach_leaf_in_aabb(aabb_t(0.0, 0.0, 100.0, 100.0), insert_to_found);
        REQUIRE(found.size() == 1);
        REQUIRE(found.count(1) == 0);
        REQUIRE(found.count(2) == 1);
    }

    SECTION("foreach_leaf_in_aabb early termination")
    {
        hash.insert(1, aabb_t(10.0, 10.0, 20.0, 20.0));
        hash.insert(2, aabb_t(15.0, 15.0, 25.0, 25.0));
        hash.insert(3, aabb_t(12.0, 12.0, 18.0, 18.0));

        int count = 0;
        auto count_only_one = [&count](SpatialHashT::leaf_index_t leaf) -> bool {
            count++;
            return false; // Stop after first leaf
        };

        hash.foreach_leaf_in_aabb(aabb_t(10.0, 10.0, 30.0, 30.0), count_only_one);
        REQUIRE(count == 1);
    }
}

TEST_CASE( "SpatialHash insertion spanning negative coordinates", "[SpatialHash]" )
{
    SpatialHash<real_t, 1021> spatialHash;

    // AABB spanning from negative to positive coordinates
    geometry::AABB<real_t> aabb{ {-100.0, -100.0}, {100.0, 100.0} };

    // leaf index
    uint32_t leaf_idx = 42;

    // Insert should not crash or access out of bounds
    REQUIRE_NOTHROW(spatialHash.insert(leaf_idx, aabb));

    // Verify the leaf is inside the spatial hash within the region
    bool found = false;
    spatialHash.foreach_leaf_in_aabb(aabb, [&found, leaf_idx](uint32_t leaf) -> bool {
        if (leaf == leaf_idx) {
            found = true;
        }
        return true;
    });

    REQUIRE(found);

    // Let's also check a point explicitly inside the AABB
    geometry::Vector<real_t> pt{ -50.0, -50.0 };
    geometry::AABB<real_t> pt_aabb{ pt, pt };
    bool pt_found = false;
    spatialHash.foreach_leaf_in_aabb(pt_aabb, [&pt_found, leaf_idx](uint32_t leaf) -> bool {
        if (leaf == leaf_idx) {
            pt_found = true;
        }
        return true;
    });

    REQUIRE(pt_found);
}
