#pragma once

#include <stdint.h>

#ifdef __3DS__
#include <3ds.h>
#endif

namespace ogm {
namespace profiler {

#ifdef __3DS__

extern uint64_t g_op_ticks[256];

class OpProfiler {
public:
    OpProfiler(int opcode) : m_opcode(opcode) {
        if (opcode < 0 || opcode >= 256) return;
        m_start = svcGetSystemTick();
    }
    ~OpProfiler() {
        uint64_t end = svcGetSystemTick();
        if (m_opcode >= 0 && m_opcode < 256) g_op_ticks[m_opcode] += (end - m_start);
    }
private:
    int m_opcode;
    uint64_t m_start;
};

#define PROFILE_OP(opcode) ogm::profiler::OpProfiler __profiler_##opcode(opcode);
#else
#define PROFILE_OP(opcode)
#endif

} // namespace profiler
} // namespace ogm
