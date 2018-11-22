cmake_minimum_required(VERSION 3.6.2)
project(lp C CXX)
# set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_compile_options(-Wall -Wextra -Wpedantic -Wno-variadic-macros -Wno-psabi -g)
add_definitions(-DSPDLOG_ENABLE_SYSLOG=1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "Disable spdlog tests")
set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "Disable cxxopts examples")
set(UDEVPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable udevpp examples")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_AUTOMOC ON)

add_subdirectory(external/udevpp)
# add_subdirectory(external/wpa_ctrl)

find_package(PkgConfig)
find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Gui CONFIG REQUIRED)
find_package(Qt5Widgets CONFIG REQUIRED)
# pkg_check_modules(PC_BCM_HOST REQUIRED QUIET bcm_host)
# link_directories(${PC_BCM_HOST_LIBRARY_DIRS})

set (SOURCE_FILES
    src/automounter.cpp
    src/logger.cpp
    src/main.cpp
    src/network.cpp
    src/qrwidget.cpp
    src/secondary_display.cpp
    src/settings.cpp
# Is needed otherwise qt moc won't work correctly see: https://github.com/bincrafters/community/issues/466
    include/secondary_display.h
    include/qrwidget.h)

add_executable(lp ${SOURCE_FILES})

set_property(TARGET lp PROPERTY CXX_STANDARD 17)
target_include_directories(lp PRIVATE include)
target_include_directories(lp PRIVATE CONAN_PKG::BcmHostWrapper)

target_link_libraries(lp PRIVATE stdc++fs)
target_link_libraries(lp PRIVATE pthread)
target_link_libraries(lp PRIVATE udevpp)
target_link_libraries(lp PRIVATE CONAN_PKG::libqrencode)
target_link_libraries(lp PRIVATE CONAN_PKG::spdlog)
target_link_libraries(lp PRIVATE CONAN_PKG::BcmHostWrapper)
target_link_libraries(lp PRIVATE Qt5::Core Qt5::Widgets Qt5::Gui)

# target_link_libraries(lp PRIVATE ${PC_BCM_HOST_LIBRARIES})
# target_include_directories(lp PRIVATE ${PC_BCM_HOST_INCLUDE_DIRS})
# target_compile_options(lp PRIVATE ${PC_BCM_CFLAGS_OTHER})
