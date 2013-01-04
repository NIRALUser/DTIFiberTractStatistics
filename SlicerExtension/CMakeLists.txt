cmake_minimum_required(VERSION 2.8.7)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME DTIAtlasFiberAnalyzer)
  set(EXTENSION_HOMEPAGE
"http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/DTIAtlasFiberAnalyzer")
  set(EXTENSION_CATEGORY "Diffusion")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
  set(EXTENSION_DESCRIPTION "This extension provides the tool DTIAtlasFiberAnalyzer
integrated in Slicer")
  set(EXTENSION_ICONURL
"http://www.nitrc.org/project/screenshot.php?group_id=312&screenshot_id=575")
  set(EXTENSION_SCREENSHOTURLS
"http://www.slicer.org/slicerWiki/images/thumb/b/b8/DTIEstim-B0-crop.png/193px-DTIEstim-B0-crop.png
http://www.slicer.org/slicerWiki/images/thumb/9/90/FiberTrack-fibers.png/138px-FiberTrack-fibers.png")
endif()

#-----------------------------------------------------------------------------
set(MODULE_NAME DTIAtlasFiberAnalyzer)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
 find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

enable_testing()

include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()



set(proj QWT)
ExternalProject_Add(${proj}
  SVN_REPOSITORY http://svn.code.sf.net/p/qwt/code/branches/qwt-6.0/
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}
  SVN_REVISION -r 1599
  DEPENDS ""
  CMAKE_GENERATOR ${gen}
  CONFIGURE_COMMAND ${QT_QMAKE_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${proj}
  INSTALL_COMMAND ""
)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QWT/lib/ DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION} )

set(proj dti_tract_stat)
ExternalProject_Add(${proj}
  SVN_REPOSITORY https://www.nitrc.org/svn/dti_tract_stat/trunk
  SVN_USERNAME slicerbot
  SVN_PASSWORD slicer
  SOURCE_DIR ${proj}
  BINARY_DIR ${proj}-build
  SVN_REVISION -r 78
  DEPENDS QWT
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    ${LOCAL_CMAKE_BUILD_OPTIONS}
    -DVTK_DIR:PATH=${VTK_DIR}
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DQWT_LIB_PATH:FILEPATH=${CMAKE_CURRENT_BINARY_DIR}/QWT/lib
    -DQWT_INCLUDE_DIR_GIVEN:PATH=${CMAKE_CURRENT_BINARY_DIR}/QWT/src
    -DCOMPILE_EXTERNAL_DTIPROCESS:BOOL=OFF
    -DCOMPILE_MERGERSTATWITHFIBER:BOOL=ON
    -DCOMPILE_FIBERCOMPARE:BOOL=ON
    -DCOMPILE_DTITRACTSTAT:BOOL=ON
    -DCOMPILE_DTIATLASFIBERANALYZER:BOOL=ON
    -DVTK_DIR:PATH=${VTK_DIR}
    -DITK_DIR:PATH=${ITK_DIR}
    -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
    -DOPT_USE_SYSTEM_VTK:BOOL=ON
    -DOPT_USE_SYSTEM_ITK:BOOL=ON
    -DOPT_USE_SYSTEM_SlicerExecutionModel:BOOL=ON
    -DQT_QMAKE_EXECUTABLE:FILEPATH=${QT_QMAKE_EXECUTABLE}
    -DSlicerExecutionModel_DIR:PATH=${SlicerExecutionModel_DIR}
  INSTALL_COMMAND ""
)

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

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
