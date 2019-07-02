cmake_minimum_required(VERSION 3.2)

project(flov)

set(flov_headers ${CMAKE_CURRENT_SOURCE_DIR}/flov.hpp)

add_library(flov INTERFACE)
target_sources(flov INTERFACE "$<BUILD_INTERFACE:${flov_headers}>")
target_include_directories(flov INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

include(external/DownloadProject/DownloadProject.cmake)
download_project(PROJ
                 nickitat_mpl
                 GIT_REPOSITORY
                 "https://github.com/nickitat/mpl.git"
                 PREFIX
                 ${CMAKE_BINARY_DIR}/nmpl
                 # CMAKE_ARGS
                 # -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}
                 LOG_DOWNLOAD ON
                 # INSTALL_DIR
                 # ${CMAKE_BINARY_DIR}/nmpl/kemlke
                 # DOWNLOAD_COMMAND    ""
                 # UPDATE_COMMAND      ""
                 # INSTALL_COMMAND     ""
                 )

add_subdirectory(${CMAKE_BINARY_DIR}/nmpl/nickitat_mpl-src)

set(additional_libs profiler nickitat_mpl)
if(NICKITAT_FLOV_WITH_BENCHMARKS)
  set(additional_libs ${additional_libs} benchmark benchmark_main)
endif()

set(additional_compiler_flags -std=c++17 -O2)
if(NICKITAT_FLOV_WITH_BENCHMARKS)
  set(additional_compiler_flags
      ${additional_compiler_flags}
      -g
      -fno-omit-frame-pointer
      -fno-rtti
      -fno-exceptions)
endif()

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set(additional_compiler_flags ${additional_compiler_flags} -pthread)
endif()
if(CMAKE_THREAD_LIBS_INIT)
  set(additional_libs ${additional_libs} ${CMAKE_THREAD_LIBS_INIT})
endif()

if(NICKITAT_FLOV_WITH_TESTS)
  add_executable(flov_tests test/test.cpp)
  target_link_libraries(flov_tests flov ${additional_libs})
  target_compile_options(flov_tests PRIVATE ${additional_compiler_flags})
endif()

if(NICKITAT_FLOV_WITH_BENCHMARKS)
  add_executable(flov_bench_std_uset benchmarks/cmp_with_std_uset.cpp)
  target_link_libraries(flov_bench_std_uset flov ${additional_libs})
  target_compile_options(flov_bench_std_uset
                         PRIVATE ${additional_compiler_flags})
endif()

# I'm currently using the following script: cmake . -Bbuild
# -DCMAKE_CXX_COMPILER=clang++ && cmake --build build && build/flov_tests
