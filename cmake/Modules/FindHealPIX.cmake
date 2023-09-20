# Find HealPIX.
# Find the HealPIX includes and library
#
# Defines the following variables:
#
#  HEALPIX_FOUND         = True if healpix found
#  HEALPIX_LIBRARIES     = Set of libraries required for linking
#  HEALPIX_INCLUDE_DIR   = Directory where to find healpix_base.h
#


find_path(HEALPIX_INCLUDE_DIR healpix_base.h
    PATHS
    /mnt/research/software/include/healpix_cxx
    /usr/include/cfitsio/
    /usr/include/
)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
set(HEALPIX_NAMES healpix_cxx sharp)

foreach (lib ${HEALPIX_NAMES})
	find_library(HEALPIX_LIBRARY_${lib} NAMES ${lib} PATHS /mnt/research/software/lib64)
    list(APPEND HEALPIX_LIBRARIES ${HEALPIX_LIBRARY_${lib}})
endforeach (lib ${HEALPIX_NAMES})

# handle the QUIETLY and REQUIRED arguments and set HEALPIX_FOUND to TRUE if.
# all listed variables are TRUE
include(FindPackageHandleCompat)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(HealPIX DEFAULT_MSG
    HEALPIX_LIBRARIES HEALPIX_INCLUDE_DIR)

if (NOT HEALPIX_FOUND)
    set(HEALPIX_LIBRARIES)
endif (NOT HEALPIX_FOUND)

mark_as_advanced(HEALPIX_LIBRARIES HEALPIX_INCLUDE_DIR)
