Project(DTIAtlasFiberAnalyzer)
cmake_minimum_required(VERSION 2.8.7)

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

option( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION
        "Build DTIAtlasFiberAnalyzer as a Slicer extension or as a Superbuild project" OFF )
if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
else()
  include("${CMAKE_CURRENT_SOURCE_DIR}/DTIAtlasFiberAnalyzer.cmake")
endif()


