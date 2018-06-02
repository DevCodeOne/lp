cmake_minimum_required(VERSION 3.6.2)
project(lp C CXX)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-variadic-macros -Wno-psabi -g)
add_definitions(-DSPDLOG_ENABLE_SYSLOG=1)

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "Disable spdlog tests")
set(UDEVPP_BUILD_EXAMPLES OFF CACHE BOOL "Disable udevpp examples")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(CMAKE_AUTOMOC ON)

add_subdirectory(external/spdlog)
add_subdirectory(external/udevpp)
# add_subdirectory(external/wpa_ctrl)

file(GLOB Qt_bin /usr/bin)
find_program(QT_MOC_EXECUTABLE qt_moc moc PATHS ${Qt_bin})
add_executable(Qt5::moc IMPORTED)
set_property(TARGET Qt5::moc PROPERTY IMPORTED_LOCATION ${QT_MOC_EXECUTABLE})

find_package(PkgConfig)
# find_package(NM REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(QRencode REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
pkg_check_modules(PC_BCM_HOST REQUIRED QUIET bcm_host)
link_directories(${PC_BCM_HOST_LIBRARY_DIRS})

file(GLOB ALL_SOURCE_FILES "src/*cpp")
qt5_wrap_cpp(secondary_display_moc include/secondary_display.h)
qt5_wrap_cpp(secondary_display_moc include/qrwidget.h)
add_executable(lp ${ALL_SOURCE_FILES} ${secondary_display_moc} ${qrwidget_moc})

target_include_directories(lp PRIVATE include)
# target_include_directories(lp PRIVATE ${NM_INCLUDE_DIRS})
target_include_directories(lp PRIVATE ${CMAKE_SYSROOT}/usr/include/c++/7.3.1)


target_link_libraries(lp PRIVATE spdlog)
target_link_libraries(lp PRIVATE udevpp)
# target_link_libraries(lp PRIVATE wpa_ctrl)
target_link_libraries(lp PRIVATE stdc++fs)
target_link_libraries(lp PRIVATE pthread)
target_link_libraries(lp PRIVATE ${JSONCPP_LIBRARIES})
target_link_libraries(lp PRIVATE ${QRENCODE_LIBRARIES})

target_link_libraries(lp PRIVATE ${PC_BCM_HOST_LIBRARIES})
target_include_directories(lp PRIVATE ${PC_BCM_HOST_INCLUDE_DIRS})
target_compile_options(lp PRIVATE ${PC_BCM_CFLAGS_OTHER})

target_link_libraries(lp PRIVATE Qt5::Gui Qt5::Core Qt5::Widgets)

add_custom_command(TARGET lp POST_BUILD
    COMMAND cat compile_commands.json | sed -e 's/--sysroot=[^\ ]*//p' > new_compile_commands.json
    COMMAND mv new_compile_commands.json compile_commands.json
    COMMENT "Removing --sysroot flags from compile_commands since the LanguageServer is not able to detect certain headers")
