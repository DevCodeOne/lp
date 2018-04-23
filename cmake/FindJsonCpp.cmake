# .rst
# FindJsonCpp
#--------
#
# Finds the JsonCpp library
#
# This will define the following variables::
#
#   JSONCPP_VERSION - libnm Version
#   JSONCPP_INCLUDE_DIRS - Include directories
#   JSONCPP_LIBRARIES  - Libraries to link to
#

find_package(PkgConfig)
pkg_check_modules(PC_JSONCPP QUIET jsoncpp)

find_library(JSONCPP_LIBRARY
    NAMES libjsoncpp.so
    PATHS ${PC_JSONCPP_LIBRARY_DIRS})

find_path(JSONCPP_INCLUDE_DIR
    NAMES json.h
    PATHS ${PC_JSONCPP_INCLUDE_DIRS}
    PATH_SUFFIXES json
    )

set(JSONCPP_VERSION ${PC_JSONCPP_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    JSONCPP
    FOUND_VAR JSONCPP_FOUND
    REQUIRED_VARS
        JSONCPP_LIBRARY
        JSONCPP_INCLUDE_DIR
    VERSION_VAR JSONCPP_VERSION
)

if (JSONCPP_FOUND)
    set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
    set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR})
endif()
