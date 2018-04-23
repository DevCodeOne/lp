# .rst
# FindQRencode
#--------
#
# Finds the QRencode library
#
# This will define the following variables::
#
#   QRENCODE_VERSION - libnm Version
#   QRENCODE_INCLUDE_DIRS - Include directories
#   QRENCODE_LIBRARIES  - Libraries to link to
#

find_package(PkgConfig)
pkg_check_modules(PC_QRENCODE QUIET libqrencode)

find_library(QRENCODE_LIBRARY
    NAMES libqrencode.so
    PATHS ${PC_QRENCODE_LIBRARY_DIRS})

find_path(QRENCODE_INCLUDE_DIR
    NAMES qrencode.h
    PATHS ${PC_QRENCODE_INCLUDE_DIRS})

set(QRENCODE_VERSION ${PC_QRENCODE_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    QRENCODE
    FOUND_VAR QRENCODE_FOUND
    REQUIRED_VARS
        QRENCODE_LIBRARY
        QRENCODE_INCLUDE_DIR
    VERSION_VAR QRENCODE_VERSION
)

if (QRENCODE_FOUND)
    set(QRENCODE_LIBRARIES ${QRENCODE_LIBRARY})
    set(QRENCODE_INCLUDE_DIRS ${QRENCODE_INCLUDE_DIR})
endif()
