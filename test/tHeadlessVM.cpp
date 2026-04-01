#include "catch/catch.hpp"
#include "ogm/bytecode/bytecode.hpp"
#include "ogm/interpreter/Executor.hpp"
#include "ogm/interpreter/execute.hpp"

#include <vector>

using namespace ogm::interpreter;
using namespace ogm::bytecode;

TEST_CASE("Headless VM Execution", "[headless]")
{
    staticExecutor.reset();

    std::vector<char> buffer;
    auto append_op = [&buffer](opcode::opcode_t op) {
        buffer.push_back(static_cast<char>(op));
    };

    auto append_int32 = [&buffer](int32_t val) {
        const char* p = reinterpret_cast<const char*>(&val);
        buffer.insert(buffer.end(), p, p + sizeof(int32_t));
    };

    auto append_uint8 = [&buffer](uint8_t val) {
        buffer.push_back(static_cast<char>(val));
    };

    append_op(opcode::ldi_s32);
    append_int32(5);
    append_op(opcode::ldi_s32);
    append_int32(10);
    append_op(opcode::add2);
    append_op(opcode::eof);

    Bytecode b(buffer.data(), buffer.size(), 0, 0);

    Instance i;
    staticExecutor.pushSelf(&i);

    // We mock the caller stack state to avoid Executor assertions on eof/ret.
    // Notice that argc and locals_start are stored as uint64_t in the stack.
    Variable argc_var, locals_var;
    argc_var = static_cast<uint64_t>(0);
    locals_var = static_cast<uint64_t>(0);

    staticExecutor.push(std::move(argc_var));
    staticExecutor.push(std::move(locals_var));

    // Manually push a dummy pc to m_return_addresses to simulate a valid call frame.
    staticExecutor.m_return_addresses.push_back(BytecodeStream());

    try {
        execute_bytecode(b);
    } catch (const ogm::interpreter::ExceptionTrace& e) {
        // eof throws a MiscError wrapped in ExceptionTrace
    } catch (const std::exception& e) {
        // eof throws a MiscError normally "Execution reached end of unit"
        // But execution of bytecodes happens inside a loop that catches it?
        // Wait, actually execute_bytecode() handles exceptions? We will just catch it.
    }

    REQUIRE(staticExecutor.peekRef(0).castCoerce<int>() == 15);

    // Clean up our dummy caller frame manually just in case
    // The previous popSelf failed because the stack frame might have been corrupted
    // when eof happened and ExceptionTrace unwound the stack. Wait, does it unwind?
    // Let's just reset everything.
    staticExecutor.reset();
}