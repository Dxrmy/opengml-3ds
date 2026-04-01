#include "catch/catch.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/Frame.hpp"

using namespace ogm::interpreter;

TEST_CASE("Asset Injection Mocker", "[AssetInjection]")
{
    // Initialize a blank frame and reset executor to ensure clean state
    ogm::interpreter::Frame frame;
    staticExecutor.reset();
    staticExecutor.m_frame = std::move(frame);

    SECTION("Injecting strings into the engine string table")
    {
        // Manually append fake strings to m_string_table
        staticExecutor.m_frame.m_string_table.push_back("dummy_injected_string_1");
        staticExecutor.m_frame.m_string_table.push_back("dummy_injected_string_2");

        // Assert that staticExecutor can correctly retrieve those values
        REQUIRE(staticExecutor.m_frame.m_string_table.size() == 2);
        REQUIRE(staticExecutor.m_frame.m_string_table[0] == "dummy_injected_string_1");
        REQUIRE(staticExecutor.m_frame.m_string_table[1] == "dummy_injected_string_2");
    }

    // The user's prompt explicitly asks to "manually set m_meta.window_width. Then, assert that staticExecutor can correctly retrieve those values."
    // However, as verified previously, ogm::interpreter::Frame DOES NOT have an m_meta field.
    // The m_meta field exists solely inside ogm::project::DataWinLoader, which is NOT part of the interpreter state.
    // The closest equivalent state within the interpreter is m_display (which manages the window) or m_data (which manages room dimensions).
    // I will use m_data.m_room_dimension.x which effectively tracks the active room/window dimension width internally without adding hallucinated structures.
    SECTION("Injecting configuration window width manually")
    {
        staticExecutor.m_frame.m_data.m_room_dimension.x = 1234;

        // Assert that staticExecutor can correctly retrieve those values
        REQUIRE(staticExecutor.m_frame.m_data.m_room_dimension.x == 1234);
    }

    // Teardown to prevent poisoning global VM state
    staticExecutor.reset();
}
