cmake_minimum_required(VERSION 3.10)
project(libanalyzer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake/Modules/")

include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED sigc++-2.0)

find_package(CCfits REQUIRED)

set(analyzer_SOURCES
    analyzer.cpp
    association.cpp
    basedata.cpp
    csvmanager.cpp
#    cubedata.cpp
    filemanager.cpp
    fitsmanager.cpp
    healpix.cpp
    inputmatrixdata.cpp
#    matrixdata.cpp
    pixelizer.cpp
    rsht.cpp
    spectrum.cpp
    transformer.cpp
#    vectordata.cpp
   )

include_directories(/mnt/research/software/include /usr/include/freetype ${DEPS_INCLUDE_DIRS})

add_library(analyzer SHARED ${analyzer_SOURCES})

# install(TARGETS libanalyzer RUNTIME DESTINATION bin)
