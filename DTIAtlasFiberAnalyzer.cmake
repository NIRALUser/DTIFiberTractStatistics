#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

IF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIAtlasFiberAnalyzer_SOURCE_DIR})
  set(LIBRARY_OUTPUT_PATH ${DTIAtlasFiberAnalyzer_BINARY_DIR}/lib CACHE PATH "Single output directory for building all libraries.")
  set(EXECUTABLE_OUTPUT_PATH ${DTIAtlasFiberAnalyzer_BINARY_DIR}/bin CACHE PATH "Single output directory for building all executables.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${DTIAtlasFiberAnalyzer_SOURCE_DIR})

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
IF(ITK_FOUND)
	INCLUDE(${ITK_USE_FILE})
ELSE(ITK_FOUND)
	MESSAGE(FATAL_ERROR "ITK not found. Please set ITK_DIR")
ENDIF(ITK_FOUND)

FIND_PACKAGE(VTK REQUIRED)
IF (VTK_FOUND)
#  SET(VTK_USE_QVTK TRUE)
#  SET(VTK_USE_GUISUPPORT TRUE)
  INCLUDE(${VTK_USE_FILE})
ELSE(VTK_FOUND)
   MESSAGE(FATAL_ERROR, "VTK not found. Please set VTK_DIR.")
ENDIF (VTK_FOUND)

if( NOT VTK_USE_QT )
  message( FATAL_ERROR "DTIAtlasFiberAnalyzer needs VTK to be build with Qt" )
endif()

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})
  INCLUDE(${QT_USE_FILE})
ELSE(QT_USE_FILE)
   MESSAGE(FATAL_ERROR, "QT not found. Please set QT_DIR.")
ENDIF(QT_USE_FILE)


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
ENDIF(COMPILE_DTITRACTSTAT)


OPTION(COMPILE_DTIATLASFIBERANALYZER "Compile DTIAtlasFiberAnalyzer." ON)
IF(COMPILE_DTIATLASFIBERANALYZER)
  include(FindQwt)
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
ENDIF(COMPILE_MERGERSTATWITHFIBER)

OPTION(COMPILE_EXTERNAL_DTIPROCESS " Compile external DTIProcess package (for fiberprocess application)." OFF)
IF(COMPILE_EXTERNAL_DTIPROCESS)
  set(proj DTIProcess)
  ExternalProject_Add(${proj}
    SVN_REPOSITORY "https://www.nitrc.org/svn/dtiprocess/branches/Slicer4Extension"
    SVN_USERNAME slicerbot
    SVN_PASSWORD slicer
    SVN_REVISION -r 139
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
