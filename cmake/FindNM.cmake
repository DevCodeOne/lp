# .rst
# FindNM
#--------
#
# Finds the NetworkManager library
#
# This will define the following variables::
#
#   NM_VERSION - libnm Version
#   NM_INCLUDE_DIRS - Include directories
#   NM_LIBRARIES  - Libraries to link to
#

find_package(PkgConfig)
pkg_check_modules(PC_NM QUIET libnm)

find_library(NM_LIBRARY
    NAMES libnm.so
    PATHS ${PC_NM_LIBRARY_DIRS})

find_path(NM_INCLUDE_DIR
    NAMES NetworkManager.h
    PATHS ${PC_NM_INCLUDE_DIRS})

set(NM_VERSION ${PC_NM_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    NM
    FOUND_VAR NM_FOUND
    REQUIRED_VARS
        NM_LIBRARY
        NM_INCLUDE_DIR
    VERSION_VAR NM_VERSION
)

if (NM_FOUND)
    set(NM_LIBRARIES ${NM_LIBRARY})
    set(NM_INCLUDE_DIRS ${PC_NM_INCLUDE_DIRS})
endif()
