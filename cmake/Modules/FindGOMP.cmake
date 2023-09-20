# - Find OpenMP
# Find the native OpenMP library
#
#  GOMP_LIBRARIES   - List of libraries when using OpenMP.
#  GOMP_FOUND       - True if OpenMP found.


set (FIND_LIBRARY_USE_LIB64_PATH TRUE)

find_library (GOMP_LIB NAMES gomp)

set (GOMP_LIBRARIES "${GOMP_LIB}")

# handle the QUIETLY and REQUIRED arguments and set GOMP_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (GOMP DEFAULT_MSG GOMP_LIBRARIES)

mark_as_advanced (GOMP_LIB)