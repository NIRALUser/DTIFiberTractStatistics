#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

set( ARCHIVE_DESTINATION lib/static )
set( LIBRARY_DESTINATION lib )
set( RUNTIME_DESTINATION bin )

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)


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

if( WIN32 )
  set( extension ".exe" )
endif()

#### Set paths for Testing subdirectory and find Slicer for packaging the extension
if( EXTENSION_SUPERBUILD_BINARY_DIR )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

if( Slicer_CLIMODULES_BIN_DIR )
  set( install_dir ${Slicer_CLIMODULES_BIN_DIR} )
endif()

add_subdirectory( Applications )

#  set( DTIAtlasFiberAnalyzer_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( FiberCompare_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( MergeStatWithFiber_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( dtitractstat_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( dtiprocess_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
set( DTIAtlasFiberAnalyzer_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
set( FiberCompare_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
set( MergeStatWithFiber_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
set( dtitractstat_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
set( dtiprocess_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )

IF(BUILD_TESTING)
  include( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)
if( EXTENSION_SUPERBUILD_BINARY_DIR )
  set( ToolsList
      ${CMAKE_CURRENT_BINARY_DIR}/${install_dir}/fiberprocess${extension}
  )
  foreach( tool ${ToolsList})
    install(PROGRAMS ${tool} DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}) 
  endforeach()
  if( UNIX AND NOT APPLE )
    set( LibsList
        ${CMAKE_CURRENT_BINARY_DIR}/${install_dir}/libDTIIO.so
        ${CMAKE_CURRENT_BINARY_DIR}/${install_dir}/libfiberprocessLib.so
    ) 
    foreach( lib ${LibsList})
      install(PROGRAMS ${lib} DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}/..) 
    endforeach()
  endif()
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()
