# Find ccfits.
# Find the native CCFITS includes and library
#
# Defines the following variables:
#
#  CCFITS_FOUND         = True if ccfits found
#  CCFITS_LIBRARIES     = Set of libraries required for linking
#  CCFITS_INCLUDE_DIR   = Directory where to find CCfits header
#


find_path(CCFITS_INCLUDE_DIR CCfits
    PATHS
    /mnt/research/software//include/
    /mnt/research/software//include/CCfits/
    /usr/include/cfitsio/
    /usr/include/
)

set(CCFITS_NAMES CCfits)

foreach (lib ${CCFITS_NAMES})
    find_library(CCFITS_LIBRARY_${lib} NAMES ${lib} PATHS /mnt/research/software/lib64)
    list(APPEND CCFITS_LIBRARIES ${CCFITS_LIBRARY_${lib}})
endforeach (lib ${CCFITS_NAMES})

# handle the QUIETLY and REQUIRED arguments and set CFITSIO_FOUND to TRUE if.
# all listed variables are TRUE
include(FindPackageHandleCompat)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CCfits DEFAULT_MSG
    CCFITS_LIBRARIES CCFITS_INCLUDE_DIR)

if (NOT CCFITS_FOUND)
    set(CCFITS_LIBRARIES)
endif (NOT CCFITS_FOUND)

mark_as_advanced(CCFITS_LIBRARIES CCFITS_INCLUDE_DIR)
