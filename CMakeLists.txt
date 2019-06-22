cmake_minimum_required(VERSION 3.2)

project(flov)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR})

include(ExternalProject)
externalproject_add(mpl
                    GIT_REPOSITORY "https://github.com/nickitat/mpl.git"
                    PREFIX ${CMAKE_BINARY_DIR}/nmpl
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}
                    LOG_DOWNLOAD ON
                    # DOWNLOAD_COMMAND    ""
                    # UPDATE_COMMAND      ""
                    # INSTALL_COMMAND     ""
                    )

find_package(nickitat_mpl REQUIRED)
add_library(nickitat_mpl STATIC IMPORTED)

add_executable(flov_tests test/test.cpp)
add_dependencies(flov_tests nickitat_mpl)
target_include_directories(flov_tests PRIVATE ${PROJECT_SOURCE_DIR}/include/mpl)

# file(GENERATE OUTPUT "includes.txt" CONTENT
# "$<TARGET_PROPERTY:nickitat_mpl,INTERFACE_INCLUDE_DIRECTORIES>\n" )

target_compile_options(flov_tests PRIVATE -std=c++17 -O2)

# I'm currently using the following script: cmake . -Bbuild
# -DCMAKE_CXX_COMPILER=clang++ && cmake --build build && build/flov_tests
