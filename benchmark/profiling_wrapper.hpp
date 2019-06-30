#pragma once

#include <gperftools/profiler.h>

struct PerfProfilingWrapper {
  PerfProfilingWrapper(const char* profileName) {
    ProfilerStart(profileName);
  }

  ~PerfProfilingWrapper() {
    ProfilerStop();
  }
};

