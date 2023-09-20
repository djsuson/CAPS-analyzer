# - Find gdal
# Find the native gdal includes and library
#
#  GDAL_INCLUDES    - where to find gdal includes
#  GDAL_LIBRARIES   - List of libraries when using gdal.
#  GDAL_FOUND       - True if gdal found.


if (GDAL_INCLUDES)
  # Already in cache, be silent
  set (GDAL_FIND_QUIETLY TRUE)
endif (GDAL_INCLUDES)

set (FIND_LIBRARY_USE_LIB64_PATH TRUE)

find_path (GDAL_INCLUDES gdal.h)

find_library (GDAL_LIB NAMES gdal)

set (GDAL_LIBRARIES "${GDAL_LIB}")

# handle the QUIETLY and REQUIRED arguments and set GSL_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (GDAL DEFAULT_MSG GDAL_LIBRARIES GDAL_INCLUDES)

mark_as_advanced (GDAL_LIB GDAL_INCLUDES)