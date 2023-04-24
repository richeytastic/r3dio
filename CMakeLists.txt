cmake_minimum_required(VERSION 3.12.2 FATAL_ERROR)

project( r3dio)

set(WITH_RIMG TRUE)
set(WITH_R3D TRUE)
set(WITH_ZLIB TRUE)
set(WITH_ASSIMP TRUE)

# Don't set true here because the library shouldn't be compiled
# with host references to IDTFConverter. Ensure set TRUE for app
# CMakeFiles and set the corresponding variable in CMake.
set(WITH_IDTF_CONVERTER FALSE)
include( "cmake/FindLibs.cmake")

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( INCLUDE_F "${INCLUDE_DIR}/${PROJECT_NAME}")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories( ${INCLUDE_DIR})
include_directories( ${INCLUDE_F})

set( INCLUDE_FILES
    "${INCLUDE_F}.h"
    "${INCLUDE_F}/AssetExporter.h"
    "${INCLUDE_F}/AssetImporter.h"
    "${INCLUDE_F}/IDTFExporter.h"
    "${INCLUDE_F}/IOFormats.h"
    "${INCLUDE_F}/IOHelpers.h"
    "${INCLUDE_F}/LatexWriter.h"
    "${INCLUDE_F}/MeshExporter.h"
    "${INCLUDE_F}/MeshImporter.h"
    "${INCLUDE_F}/OBJExporter.h"
    "${INCLUDE_F}/PDFGenerator.h"
    "${INCLUDE_F}/PLYExporter.h"
    "${INCLUDE_F}/TGAImage.h"
    "${INCLUDE_F}/U3DExporter.h"
    )

set( SRC_FILES
    "${SRC_DIR}/AssetExporter.cpp"
    "${SRC_DIR}/AssetImporter.cpp"
    "${SRC_DIR}/IDTFExporter.cpp"
    "${SRC_DIR}/IOFormats.cpp"
    "${SRC_DIR}/IOHelpers.cpp"
    "${SRC_DIR}/LatexWriter.cpp"
    "${SRC_DIR}/MeshExporter.cpp"
    "${SRC_DIR}/MeshImporter.cpp"
    "${SRC_DIR}/OBJExporter.cpp"
    "${SRC_DIR}/PDFGenerator.cpp"
    "${SRC_DIR}/PLYExporter.cpp"
    "${SRC_DIR}/TGAImage.cpp"
    "${SRC_DIR}/U3DExporter.cpp"
    )

add_library( ${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})
include( "cmake/LinkLibs.cmake")
