#include "catch/catch.hpp"
#include "ogm/common/types.hpp"
#include "ogm/collision/SpatialHash.hpp"

#include <iostream>

using namespace ogm;
using namespace ogm::collision;

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
