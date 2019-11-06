# =============================================================================
# The r3dio CMake configuration file.
#
#           ** File generated automatically, DO NOT MODIFY! ***

# To use from an external project, in your project's CMakeLists.txt add:
#   FIND_PACKAGE( r3dio REQUIRED)
#   INCLUDE_DIRECTORIES( r3dio ${r3dio_INCLUDE_DIRS})
#   LINK_DIRECTORIES( ${r3dio_LIBRARY_DIR})
#   TARGET_LINK_LIBRARIES( MY_TARGET_NAME ${r3dio_LIBRARIES})
#
# This module defines the following variables:
#   - r3dio_FOUND         : True if r3dio is found.
#   - r3dio_ROOT_DIR      : The root directory where r3dio is installed.
#   - r3dio_INCLUDE_DIRS  : The r3dio include directories.
#   - r3dio_LIBRARY_DIR   : The r3dio library directory.
#   - r3dio_LIBRARIES     : The r3dio imported libraries to link to.
#
# =============================================================================

get_filename_component( r3dio_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component( r3dio_ROOT_DIR  "${r3dio_CMAKE_DIR}"           PATH)

set( r3dio_INCLUDE_DIRS "${r3dio_ROOT_DIR}/../include" CACHE PATH "The r3dio include directories.")
set( r3dio_LIBRARY_DIR  "${r3dio_ROOT_DIR}"            CACHE PATH "The r3dio library directory.")

include( "${CMAKE_CURRENT_LIST_DIR}/Macros.cmake")
get_library_suffix( _lsuff)
set( _hints r3dio${_lsuff} libr3dio${_lsuff})
find_library( r3dio_LIBRARIES NAMES ${_hints} PATHS "${r3dio_LIBRARY_DIR}/static" "${r3dio_LIBRARY_DIR}")
set( r3dio_LIBRARIES     ${r3dio_LIBRARIES}         CACHE FILEPATH "The r3dio imported libraries to link to.")

# handle QUIETLY and REQUIRED args and set r3dio_FOUND to TRUE if all listed variables are TRUE
include( "${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake")
find_package_handle_standard_args( r3dio r3dio_FOUND r3dio_LIBRARIES r3dio_INCLUDE_DIRS)
