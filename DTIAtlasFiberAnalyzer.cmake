project( DTIAtlasFiberAnalyzerProject )

#-----------------------------------------------------------------------------

if(BUILD_TESTING)
  include( CTest )
endif(BUILD_TESTING)

SETIFEMPTY( ARCHIVE_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib/static )
SETIFEMPTY( LIBRARY_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib )
SETIFEMPTY( RUNTIME_DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin )

SETIFEMPTY(INSTALL_RUNTIME_DESTINATION bin)
SETIFEMPTY(INSTALL_LIBRARY_DESTINATION lib)
SETIFEMPTY(INSTALL_ARCHIVE_DESTINATION lib)
set( install_dir ${INSTALL_RUNTIME_DESTINATION} )

find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
include(${ITK_USE_FILE})

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
if( VTK_MAJOR_VERSION VERSION_LESS 6 AND NOT VTK_USE_QT )
  message( FATAL_ERROR "DTIAtlasFiberAnalyzer needs VTK to be built with Qt" )
endif()
if( VTK_MAJOR_VERSION VERSION_GREATER 5 ) 
  list( FIND VTK_MODULES_ENABLED "vtkGUISupportQt" vtkQtpos )
  if( vtkQtpos LESS 0 )
    message( FATAL_ERROR "DTIAtlasFiberAnalyzer needs VTK to be built with Qt" )
  endif()
endif()

find_package(Qt5 COMPONENTS Core Gui Network Xml REQUIRED)

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

option(EXECUTABLES_ONLY "Build only executables (CLI)" ON)
if( ${EXECUTABLES_ONLY} )
  set( STATIC "EXECUTABLE_ONLY" )
  set( STATIC_LIB "STATIC" )
else()
  set( STATIC_LIB "SHARED" )
endif()

add_subdirectory( Applications )

find_package(DTIProcess REQUIRED)

set( ToolsList
  ${DTIProcess_fiberprocess_EXECUTABLE}
)


if( EXECUTABLES_ONLY )
  foreach( tool ${ToolsList})
    message(STATUS "Install: ${tool}")
    install(PROGRAMS ${tool} DESTINATION ${INSTALL_RUNTIME_DESTINATION})
  endforeach()
endif()

