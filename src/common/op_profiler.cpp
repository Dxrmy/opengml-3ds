#include "ogm/common/op_profiler.hpp"

namespace ogm {
namespace profiler {

#ifdef __3DS__
uint64_t g_op_ticks[256] = {0};
#endif

}
}
