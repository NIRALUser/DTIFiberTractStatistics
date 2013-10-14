Project(DTIAtlasFiberAnalyzer)
cmake_minimum_required(VERSION 2.8.7)

include(${CMAKE_CURRENT_SOURCE_DIR}/Common.cmake)

option( DTIAtlasFiberAnalyzer_SuperBuild
        "Build DTIAtlasFiberAnalyzer as a Superbuild project" ON )
if( DTIAtlasFiberAnalyzer_SuperBuild )
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
else()
  include("${CMAKE_CURRENT_SOURCE_DIR}/DTIAtlasFiberAnalyzer.cmake")
endif()


