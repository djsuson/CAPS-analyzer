cmake_minimum_required(VERSION 3.10)
project(libgraphics)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../../cmake/Modules/")

include(FindPkgConfig)
pkg_check_modules(DEPS REQUIRED sigc++-2.0)
find_package(lunar REQUIRED)

set(graphics_SOURCES
    datamap.cpp
    dataspectrum.cpp
    font_mroman.cpp
    font_roman.cpp
    grapher.cpp
    mapper.cpp
    mollweide.cpp
    progress.cpp
    raster.cpp
    coordtransformer.cpp
   )

include_directories(/mnt/research/software/include /usr/include/freetype ${DEPS_INCLUDE_DIRS})

add_library(graphics SHARED ${graphics_SOURCES})

# install(TARGETS libanalyzer RUNTIME DESTINATION bin)
