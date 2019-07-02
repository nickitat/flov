#pragma once

#include <gperftools/profiler.h>

namespace flov {
namespace bench {

struct PerfProfilingWrapper {
  PerfProfilingWrapper(const char* profileName) {
    ProfilerStart(profileName);
  }

  ~PerfProfilingWrapper() {
    ProfilerStop();
  }
};

}  // namespace bench
}  // namespace flov
