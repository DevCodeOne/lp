cmake_minimum_required(VERSION 3.6.2)
project(lp C CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_compile_options(-Wall -Wextra -Wpedantic -g)
add_definitions(-std=c++17)
add_definitions(-DSPDLOG_ENABLE_SYSLOG=1)

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "Disable spdlog tests")
set(UDEVPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable udevpp examples")

add_subdirectory(external/spdlog)
add_subdirectory(external/udevpp)

find_package(PkgConfig)
# find_package(NM REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(QRencode REQUIRED)
pkg_check_modules(PC_BCM_HOST REQUIRED QUIET bcm_host)
link_directories(${PC_BCM_HOST_LIBRARY_DIRS})

file(GLOB ALL_SOURCE_FILES "src/*cpp")
add_executable(lp ${ALL_SOURCE_FILES})
target_include_directories(lp PRIVATE include)
# target_include_directories(lp PRIVATE ${NM_INCLUDE_DIRS})

target_link_libraries(lp PRIVATE spdlog)
target_link_libraries(lp PRIVATE udevpp)

message(STATUS "directories : " ${PC_BCM_HOST_LIBRARY_DIRS})
target_link_libraries(lp PRIVATE ${PC_BCM_HOST_LIBRARIES})
target_include_directories(lp PRIVATE ${PC_BCM_HOST_INCLUDE_DIRS})
target_compile_options(lp PRIVATE ${PC_BCM_CFLAGS_OTHER})
