Project(DTIAtlasFiberAnalyzer)
cmake_minimum_required(VERSION 2.8.7)

set(Slicer_SKIP_PROJECT_COMMAND ON)
#-----------------------------------------------------------------------------
set(EXTENSION_NAME DTIAtlasFiberAnalyzer)
set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/DTIAtlasFiberAnalyzer")
set(EXTENSION_CATEGORY "Diffusion")
set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
set(EXTENSION_DESCRIPTION "This extension provides the tool DTIAtlasFiberAnalyzer integrated in Slicer")
set(EXTENSION_ICONURL "http://www.nitrc.org/project/screenshot.php?group_id=312&screenshot_id=575")
set(EXTENSION_SCREENSHOTURLS "http://www.nitrc.org/project/screenshot.php?group_id=403&screenshot_id=601 http://www.slicer.org/slicerWiki/images/thumb/9/90/FiberTrack-fibers.png/138px-FiberTrack-fibers.png")
set(EXTENSION_STATUS "Beta")
set(EXTENSION_DEPENDS "DTIProcess") # Specified as a space separated list or 'NA' if any
set(EXTENSION_BUILD_SUBDIRECTORY dti_tract_stat-build )

find_package(Git REQUIRED)

option( BUILD_TESTING
        "Build the testing tree" ON )
option( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION
        "Build DTIAtlasFiberAnalyzer as a Slicer extension or as a Superbuild project" OFF )
if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()


#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

IF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIAtlasFiberAnalyzer_SOURCE_DIR})
  set(LIBRARY_OUTPUT_PATH ${DTIAtlasFiberAnalyzer_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
  set(EXECUTABLE_OUTPUT_PATH ${DTIAtlasFiberAnalyzer_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIAtlasFiberAnalyzer_SOURCE_DIR})

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
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    INSTALL_COMMAND ""
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
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DQWT_INCLUDE_DIR:PATH=${QWT_INCLUDE_DIR}
      -DQWT_LIBRARY:STRING=${QWT_LIBRARY}
      -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    INSTALL_COMMAND ""
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
    DEPENDS ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    INSTALL_COMMAND ""
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_MERGERSTATWITHFIBER)

OPTION(COMPILE_EXTERNAL_DTIPROCESS " Compile external DTIProcess package (for fiberprocess application)." OFF)
IF(COMPILE_EXTERNAL_DTIPROCESS)
  set(proj DTIProcess)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/branches/Slicer4Extension"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SVN_REVISION -r 138
    SOURCE_DIR ${proj}
    BINARY_DIR ${proj}-build
    DEPENDS ${ITK_DEPEND} ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DBUILD_dwiAtlas:BOOL=OFF
    INSTALL_COMMAND ""
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
    DEPENDS ${VTK_DEPEND} ${SlicerExecutionModel_DEPEND} ${ITK_DEPEND}
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${LOCAL_CMAKE_BUILD_OPTIONS}
      -DITK_DIR:PATH=${ITK_DIR}
      -DVTK_DIR:PATH=${VTK_DIR}
      -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
      -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND ""
  )
  FORCE_BUILD_CHECK(${proj})  
ENDIF(COMPILE_FIBERCOMPARE)


if( EXTENSION_SUPERBUILD_BINARY_DIR )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  set( DTIAtlasFiberAnalyzer_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
  set( FiberCompare_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
  set( MergeStatWithFiber_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
  set( dtitractstat_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
  set( dtiprocess_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
else()
  set( DTIAtlasFiberAnalyzer_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/DTIAtlasFiberAnalyzer-build/bin/ )
  set( FiberCompare_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/FiberCompare-build/bin/ )
  set( MergeStatWithFiber_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/MergeStatWithFiber-build/bin/ )
  set( dtitractstat_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/dtitractstat-build/bin/ )
  set( dtiprocess_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/DTIProcess-build/bin/ )
endif()

IF(BUILD_TESTING)
  include( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

if( EXTENSION_SUPERBUILD_BINARY_DIR )
  set( ToolsList
  dtitractstat
  DTIAtlasFiberAnalyzer
  MergeStatWithFiber
  FiberCompare
  fiberprocess
  ) 
  foreach( tool ${ToolsList})
    install(PROGRAMS ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY}/${tool} DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}) 
  endforeach()
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()
