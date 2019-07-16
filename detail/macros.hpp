#pragma once

// useful when profiling
#define FLOV_NOINLINE __attribute__((noinline))

#ifdef FLOV_WITH_DEBUG_INFO
#include <cassert>

#define FLOV_ASSERT(x) assert((x))
#else
#define FLOV_ASSERT(x)
#endif

#ifdef FLOV_WITH_DEBUG_INFO
#define FLOV_DEBUG_INFO(x) (x)
#else
#define FLOV_DEBUG_INFO(x)
#endif
