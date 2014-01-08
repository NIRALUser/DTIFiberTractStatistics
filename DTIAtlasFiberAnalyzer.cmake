#-----------------------------------------------------------------------------
set(MODULE_NAME ${EXTENSION_NAME}) # Do not use 'project()'
set(MODULE_TITLE ${MODULE_NAME})

#### Set paths for Testing subdirectory and find Slicer for packaging the extension
if( EXTENSION )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

if( EXTENSION )
  set( ARCHIVE_DESTINATION lib/static )
  set( LIBRARY_DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_LIBRARY_DESTINATION} )
  set( RUNTIME_DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION} )
else()
  set( ARCHIVE_DESTINATION lib/static )
  set( LIBRARY_DESTINATION lib )
  set( RUNTIME_DESTINATION bin )
endif()


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
  set( fileextension ".exe" )
endif()

option(EXECUTABLES_ONLY "Build only executables (CLI)" OFF)
if( ${EXECUTABLES_ONLY} )

  set( STATIC "EXECUTABLE_ONLY" )

  set( STATIC_LIB "STATIC" )

else()

  set( STATIC_LIB "SHARED" )

endif()


if( Slicer_CLIMODULES_BIN_DIR )
  set( install_dir ${Slicer_CLIMODULES_BIN_DIR} )
else()
  set( install_dir ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION} )
endif()

add_subdirectory( Applications )

#  set( DTIAtlasFiberAnalyzer_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( FiberCompare_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( MergeStatWithFiber_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( dtitractstat_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#  set( dtiprocess_BUILD_DIR ${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY} )
#set( DTIAtlasFiberAnalyzer_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
#set( FiberCompare_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
#set( MergeStatWithFiber_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
#set( dtitractstat_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )
#set( dtiprocess_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin/ )

IF(BUILD_TESTING)
  include( CTest )
  ADD_SUBDIRECTORY(Testing)
ENDIF(BUILD_TESTING)

set( ToolsList
    ${CMAKE_CURRENT_BINARY_DIR}/${install_dir}/fiberprocess${fileextension}
)
foreach( tool ${ToolsList})
  install(PROGRAMS ${tool} DESTINATION ${RUNTIME_DESTINATION})
endforeach()
if( EXTENSION )
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
endif()
