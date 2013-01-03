Project(DTIFiberTractMain)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(VERSION 2.8)

IF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIFiberTractMain_SOURCE_DIR})
  set(LIBRARY_OUTPUT_PATH ${DTIFiberTractMain_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
  set(EXECUTABLE_OUTPUT_PATH ${DTIFiberTractMain_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIFiberTractMain_SOURCE_DIR})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Install path prefix, prepended onto install directories." FORCE)
set(CMAKE_HELPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
include(${CMAKE_HELPER_DIR}/ExternalProject.cmake)
include(${CMAKE_HELPER_DIR}/CMakeBuildMacros.cmake)
include(${CMAKE_HELPER_DIR}/PreventInSourceBuilds.cmake)
include(${CMAKE_HELPER_DIR}/CMakeCommonExternalDefinitions.cmake)

#External Projects
include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()

PACKAGE_NEEDS_ITK("${LOCAL_CMAKE_BUILD_OPTIONS}" "${gen}")
PACKAGE_NEEDS_VTKWITHQT("${LOCAL_CMAKE_BUILD_OPTIONS}" "${gen}")
PACKAGE_NEEDS_SlicerExecutionModel("${LOCAL_CMAKE_BUILD_OPTIONS}" "${gen}")

OPTION(COMPILE_DTITRACTSTAT "Compile dtitractstat." ON)
IF(COMPILE_DTITRACTSTAT)
  set(proj dtitractstat)
  ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Applications/${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${GenerateCLP_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_DTITRACTSTAT)


OPTION(COMPILE_DTIATLASFIBERANALYZER "Compile DTIAtlasFiberAnalyzer." ON)
IF(COMPILE_DTIATLASFIBERANALYZER)
  include(${CMAKE_HELPER_DIR}/FindQwt.cmake)
  set(proj DTIAtlasFiberAnalyzer)
  ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Applications/${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${GenerateCLP_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
      -DQWT_INCLUDE_DIR:PATH=${QWT_INCLUDE_DIR}
      -DQWT_LIBRARY:STRING=${QWT_LIBRARY}
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_DTIATLASFIBERANALYZER)

OPTION(COMPILE_MERGERSTATWITHFIBER "Compile MergeStatWithFiber." ON)
IF(COMPILE_MERGERSTATWITHFIBER)
  set(proj MergeStatWithFiber)
  ExternalProject_Add(${proj}
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Applications/${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${VTK_DEPEND} ${GenerateCLP_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_MERGERSTATWITHFIBER)

OPTION(COMPILE_EXTERNAL_DTIPROCESS " Compile external DTIProcess package (for fiberprocess application)." OFF)
IF(COMPILE_EXTERNAL_DTIPROCESS)
  set(proj DTIProcess)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/trunk"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${GenerateCLP_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DGenerateCLP_DIR:PATH=${GenerateCLP_DIR}
      -DBUILDSEMFROMDTIProcess:BOOL=OFF
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_dwiAtlas:BOOL=OFF
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  )
  FORCE_BUILD_CHECK(${proj})
ENDIF(COMPILE_EXTERNAL_DTIPROCESS)

OPTION(COMPILE_FIBERCOMPARE " Compile FiberCompare." OFF)
IF(COMPILE_FIBERCOMPARE)
  set(proj FiberCompare)
  ExternalProject_Add(${proj}
 DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Applications/${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${VTK_DEPEND} ${GenerateCLP_DEPEND} ${ITK_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    INSTALL_COMMAND ""
    INSTALL_DIR ${CMAKE_INSTALL_PREFIX}
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_FIBERCOMPARE)
