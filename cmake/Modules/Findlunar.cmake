# Find lunar.
# Find the native lunar includes and library
#
# Defines the following variables:
#
#  LUNAR_FOUND         = True if lunar found
#  LUNAR_LIBRARIES     = Set of libraries required for linking
#  LUNAR_INCLUDE_DIR   = Directory where to find lunar.h
#


find_path(LUNAR_INCLUDE_DIR lunar.h
    PATHS
    /mnt/research/software/include/
    /usr/include/
)

set(LUNAR_NAMES lunar)

foreach (lib ${LUNAR_NAMES})
    find_library(LUNAR_LIBRARY_${lib} NAMES ${lib} PATHS /mnt/research/software/lib64/)
    list(APPEND LUNAR_LIBRARIES ${LUNAR_LIBRARY_${lib}})
endforeach (lib ${LUNAR_NAMES})

# handle the QUIETLY and REQUIRED arguments and set LUNAR_FOUND to TRUE if.
# all listed variables are TRUE
include(FindPackageHandleCompat)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(lunar DEFAULT_MSG
    LUNAR_LIBRARIES LUNAR_INCLUDE_DIR)

if (NOT LUNAR_FOUND)
    set(LUNAR_LIBRARIES)
endif (NOT LUNAR_FOUND)

mark_as_advanced(LUNAR_LIBRARIES LUNAR_INCLUDE_DIR)
