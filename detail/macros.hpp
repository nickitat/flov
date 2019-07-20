#pragma once

// useful when profiling
#define FLOV_NOINLINE __attribute__((noinline))

#ifdef FLOV_DEBUG_BUILD
#include <cassert>

#define FLOV_ASSERT(x) assert((x))
#else
#define FLOV_ASSERT(x)
#endif
