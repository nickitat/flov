cmake_minimum_required(VERSION 3.2)

project(flov)

# set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_SOURCE_DIR}/build/nmpl)

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

# find_package(nickitat_mpl REQUIRED) add_library(nickitat_mpl GLOBAL IMPORTED
# INTERFACE)

add_executable(flov_tests test/test.cpp)
# add_dependencies(flov_tests nickitat_mpl)
target_link_libraries(flov_tests nickitat_mpl)
# target_include_directories( flov_tests PUBLIC
# $<TARGET_PROPERTY:nickitat_mpl,INTERFACE_INCLUDE_DIRECTORIES>)
# ${PROJECT_SOURCE_DIR}/include/mpl) target_include_directories(flov_tests
# PRIVATE ${CMAKE_BINARY_DIR}/nmpl/nickitat_mpl-src)

# file(GENERATE OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/includes.txt CONTENT
# "$<TARGET_PROPERTY:nickitat_mpl,INTERFACE_INCLUDE_DIRECTORIES>\n")

target_compile_options(flov_tests PRIVATE -std=c++17 -O2)

# I'm currently using the following script: cmake . -Bbuild
# -DCMAKE_CXX_COMPILER=clang++ && cmake --build build && build/flov_tests
