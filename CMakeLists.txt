cmake_minimum_required(VERSION 3.6.2)
project(lp C CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_compile_options(-Wall -Wextra -Wpedantic -Wno-psabi -g)
add_definitions(-DSPDLOG_ENABLE_SYSLOG=1)

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "Disable spdlog tests")
set(UDEVPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable udevpp examples")

add_subdirectory(external/spdlog)
add_subdirectory(external/udevpp)
add_subdirectory(external/wpa_ctrl)

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
target_include_directories(lp PRIVATE ${CMAKE_SYSROOT}/usr/include/c++/7.3.1)

target_compile_options(lp PRIVATE -std=c++17)

target_link_libraries(lp PRIVATE spdlog)
target_link_libraries(lp PRIVATE udevpp)
target_link_libraries(lp PRIVATE wpa_ctrl)
target_link_libraries(lp PRIVATE stdc++fs)
target_link_libraries(lp PRIVATE pthread)

target_link_libraries(lp PRIVATE ${PC_BCM_HOST_LIBRARIES})
target_include_directories(lp PRIVATE ${PC_BCM_HOST_INCLUDE_DIRS})
target_compile_options(lp PRIVATE ${PC_BCM_CFLAGS_OTHER})

add_custom_command(TARGET lp POST_BUILD
    COMMAND cat compile_commands.json | sed -e 's/--sysroot=[^\ ]*//p' > new_compile_commands.json
    COMMAND mv new_compile_commands.json compile_commands.json
    COMMENT "Removing --sysroot flags from compile_commands since the LanguageServer is not able to detect certain headers")
