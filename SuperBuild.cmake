set(PRIMARY_PROJECT_NAME DTIAtlasFiberAnalyzer)

set( ${PRIMARY_PROJECT_NAME}_USE_QT ON )

set(EXTERNAL_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Select where external packages will be downloaded" )
set(EXTERNAL_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} CACHE PATH "Select where external packages will be compiled and installed" )

#-----------------------------------------------------------------------------
# Extension option(s)
#-----------------------------------------------------------------------------
if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  #-----------------------------------------------------------------------------
  set(EXTENSION_NAME DTIAtlasFiberAnalyzer)
  set(EXTENSION_HOMEPAGE "http://www.nitrc.org/projects/dti_tract_stat")
  set(EXTENSION_CATEGORY "Diffusion")
  set(EXTENSION_CONTRIBUTORS "Francois Budin (UNC)")
  set(EXTENSION_DESCRIPTION "This extension provides the tool DTIAtlasFiberAnalyzer integrated in Slicer")
  set(EXTENSION_ICONURL "http://www.nitrc.org/project/screenshot.php?group_id=403&screenshot_id=768")
  set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/images/thumb/2/20/Screenshot-DTI_Atlas_Fiber_Analyser.png/745px-Screenshot-DTI_Atlas_Fiber_Analyser.png http://wiki.slicer.org/slicerWiki/images/thumb/6/6e/DTIAtlasFiberAnalyzerGenu_profile_FA.png/800px-DTIAtlasFiberAnalyzerGenu_profile_FA.png")
  set(EXTENSION_STATUS "")
  set(EXTENSION_DEPENDS "DTIProcess") # Specified as a space separated list or 'NA' if any
  set(EXTENSION_BUILD_SUBDIRECTORY . )
  find_package(Slicer REQUIRED)
  set( EXTENSION TRUE)
  #set( COMPILE_EXTERNAL_DTIPROCESS OFF CACHE BOOL "Compile external DTIProcess package (for fiberprocess application)." FORCE )
  set( Slicer_USE_PYTHONQT FALSE )  
  set( USE_SYSTEM_ITK ON CACHE BOOL "Build using an externally defined version of ITK" FORCE )
  set( USE_SYSTEM_VTK ON CACHE BOOL "Build using an externally defined version of VTK" FORCE )
  #VTK_VERSION_MAJOR is define but not a CACHE variable
  set( VTK_VERSION_MAJOR ${VTK_VERSION_MAJOR} CACHE STRING "Choose the expected VTK major version to build Slicer (5 or 6).")
  set( USE_SYSTEM_SlicerExecutionModel ON CACHE BOOL "Build using an externally defined version of SlicerExecutionModel" FORCE )
  # DTIProcess_DIR is set because DTI-Reg is defined as dependent of the extension DTIProcess
  set(USE_SYSTEM_DTIProcess ON CACHE BOOL "Build using an externally defined version of DTIProcess" FORCE)
  
endif()

option(USE_SYSTEM_ITK "Build using an externally defined version of ITK" OFF)
option(USE_SYSTEM_SlicerExecutionModel "Build using an externally defined version of SlicerExecutionModel"  OFF)
option(USE_SYSTEM_VTK "Build using an externally defined version of VTK" OFF)
option(USE_SYSTEM_DTIProcess "Build using an externally defined version of DTIProcess" OFF)
option(USE_SYSTEM_FADTTS "Build using an externally defined version of FADTTS" OFF)

#-----------------------------------------------------------------------------
# Superbuild option(s)
#-----------------------------------------------------------------------------
option(BUILD_STYLE_UTILS "Build uncrustify, cppcheck, & KWStyle" OFF)
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_Uncrustify "Use system Uncrustify program" OFF
  "BUILD_STYLE_UTILS" OFF
  )
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_KWStyle "Use system KWStyle program" OFF
  "BUILD_STYLE_UTILS" OFF
  )
CMAKE_DEPENDENT_OPTION(
  USE_SYSTEM_Cppcheck "Use system Cppcheck program" OFF
  "BUILD_STYLE_UTILS" OFF
  )

option(EXECUTABLES_ONLY "Build the tools and the tools' libraries statically" ON)
#------------------------------------------------------------------------------
# ${PRIMARY_PROJECT_NAME} dependency list
#------------------------------------------------------------------------------

set(ITK_VERSION_MAJOR 4)
set(ITK_EXTERNAL_NAME ITKv${ITK_VERSION_MAJOR})

set(${PRIMARY_PROJECT_NAME}_DEPENDENCIES ${ITK_EXTERNAL_NAME} VTK SlicerExecutionModel DTIProcess QtToCppXML FADTTS)

if(BUILD_STYLE_UTILS)
  list(APPEND ${PRIMARY_PROJECT_NAME}_DEPENDENCIES Cppcheck KWStyle Uncrustify)
endif()


#-----------------------------------------------------------------------------
# Define Superbuild global variables
#-----------------------------------------------------------------------------

# This variable will contain the list of CMake variable specific to each external project
# that should passed to ${CMAKE_PROJECT_NAME}.
# The item of this list should have the following form: <EP_VAR>:<TYPE>
# where '<EP_VAR>' is an external project variable and TYPE is either BOOL, STRING, PATH or FILEPATH.
# TODO Variable appended to this list will be automatically exported in ${PRIMARY_PROJECT_NAME}Config.cmake,
# prefix '${PRIMARY_PROJECT_NAME}_' will be prepended if it applies.
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS)

# The macro '_expand_external_project_vars' can be used to expand the list of <EP_VAR>.
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS) # List of CMake args to configure BRAINS
set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES) # List of CMake variable names

# Convenient macro allowing to expand the list of EP_VAR listed in ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
# The expanded arguments will be appended to the list ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS
# Similarly the name of the EP_VARs will be appended to the list ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES.
macro(_expand_external_project_vars)
  set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS "")
  set(${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES "")
  foreach(arg ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS})
    string(REPLACE ":" ";" varname_and_vartype ${arg})
    set(target_info_list ${target_info_list})
    list(GET varname_and_vartype 0 _varname)
    list(GET varname_and_vartype 1 _vartype)
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS -D${_varname}:${_vartype}=${${_varname}})
    list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES ${_varname})
  endforeach()
endmacro()



#-----------------------------------------------------------------------------
# Common external projects CMake variables
#-----------------------------------------------------------------------------
list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
  MAKECOMMAND:STRING
  CMAKE_SKIP_RPATH:BOOL
  CMAKE_BUILD_TYPE:STRING
  BUILD_SHARED_LIBS:BOOL
  CMAKE_CXX_COMPILER:PATH
  CMAKE_CXX_FLAGS_RELEASE:STRING
  CMAKE_CXX_FLAGS_DEBUG:STRING
  CMAKE_CXX_FLAGS:STRING
  CMAKE_C_COMPILER:PATH
  CMAKE_C_FLAGS_RELEASE:STRING
  CMAKE_C_FLAGS_DEBUG:STRING
  CMAKE_C_FLAGS:STRING
  CMAKE_SHARED_LINKER_FLAGS:STRING
  CMAKE_MODULE_LINKER_FLAGS:STRING
  CMAKE_EXE_LINKER_FLAGS:STRING
  CMAKE_GENERATOR:STRING
  CMAKE_EXTRA_GENERATOR:STRING
  CMAKE_INSTALL_PREFIX:PATH
  CMAKE_LIBRARY_OUTPUT_DIRECTORY:PATH
  CMAKE_ARCHIVE_OUTPUT_DIRECTORY:PATH
  CMAKE_RUNTIME_OUTPUT_DIRECTORY:PATH
  CMAKE_BUNDLE_OUTPUT_DIRECTORY:PATH
  CTEST_NEW_FORMAT:BOOL
  MEMORYCHECK_COMMAND_OPTIONS:STRING
  MEMORYCHECK_COMMAND:PATH
  SITE:STRING
  BUILDNAME:STRING
  Subversion_SVN_EXECUTABLE:FILEPATH
  GIT_EXECUTABLE:FILEPATH
  USE_GIT_PROTOCOL:BOOL
  )

_expand_external_project_vars()
set(COMMON_EXTERNAL_PROJECT_ARGS ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS})
set(extProjName ${PRIMARY_PROJECT_NAME})
set(proj        ${PRIMARY_PROJECT_NAME})
SlicerMacroCheckExternalProjectDependency(${PRIMARY_PROJECT_NAME})

#-----------------------------------------------------------------------------
# Set CMake OSX variable to pass down the external project
#-----------------------------------------------------------------------------
set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
if(APPLE)
  list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
    -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
    -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET})
endif()




#-----------------------------------------------------------------------------
# Add external project CMake args
#-----------------------------------------------------------------------------
list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
  BUILD_TESTING:BOOL
  EXTENSION:BOOL
  CMAKE_MODULE_PATH:PATH
  EXTENSION_NAME:STRING
  MIDAS_PACKAGE_EMAIL:STRING
  MIDAS_PACKAGE_API_KEY:STRING
  Slicer_DIR:PATH
  )

_expand_external_project_vars()
set(COMMON_EXTERNAL_PROJECT_ARGS ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS})


if(verbose)
  message("Inner external project args:")
  foreach(arg ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS})
    message("  ${arg}")
  endforeach()
endif()

string(REPLACE ";" "^" ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES "${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES}")

if(verbose)
  message("Inner external project argnames:")
  foreach(argname ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARNAMES})
    message("  ${argname}")
  endforeach()
endif()


set(proj DTIAtlasFiberAnalyzer-inner)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${proj}-build
  DEPENDS ${${PRIMARY_PROJECT_NAME}_DEPENDENCIES}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS    
    -DCOMPILE_MERGERSTATWITHFIBER:BOOL=ON
    -DCOMPILE_FIBERCOMPARE:BOOL=ON
    -DCOMPILE_DTITRACTSTAT:BOOL=ON
    -DCOMPILE_DTIATLASFIBERANALYZER:BOOL=ON
    -DDTIAtlasFiberAnalyzer_SuperBuild:BOOL=OFF
     ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
     ${COMMON_EXTERNAL_PROJECT_ARGS}
    -DEXECUTABLES_ONLY:BOOL=${EXECUTABLES_ONLY}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/${proj}-install
    -DCMAKE_PREFIX_PATH:PATH=${Qt5_DIR}
    -DQtToCppXML_DIR:PATH=${QtToCppXML_DIR}
)

## Force rebuilding of the main subproject every time building from super structure
ExternalProject_Add_Step(${proj} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_CURRENT_BUILD_DIR}/${proj}-prefix/src/${proj}-stamp/${proj}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
  )

#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------
# Extensions with Superbuild that have different libraries than the ones included in Slicer
# must be packaged in outer-build directory (since Experimental and other targets created
# by Slicer reconfigure the project from scratch and erase paths given from the outer-build directory
# to the inner-build directory.

if( DTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION )
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
  set( CLIs DTIAtlasFiberAnalyzer dtitractstat FiberCompare FiberPostProcess MergeStatWithFiber )
  foreach( VAR ${CLIs})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/bin/${VAR}${fileextension} DESTINATION ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION})
  endforeach()
  #set( NOCLI_INSTALL_DIR ${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}/../hidden-cli-modules)
  #install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/bin/DTIAtlasFiberAnalyzer${fileextension} DESTINATION ${NOCLI_INSTALL_DIR})
  set(CPACK_INSTALL_CMAKE_PROJECTS "${CPACK_INSTALL_CMAKE_PROJECTS};${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
  include(${Slicer_EXTENSION_CPACK})
  # For the tests, we manually import the targets built in the inner-build directory
  foreach( VAR ${CLIs} FiberPostProcessTest)
    add_executable(${VAR} IMPORTED)
    set_property(TARGET ${VAR} PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/bin/${VAR}${fileextension})
  endforeach()
  IF(BUILD_TESTING)
    include( CTest )
    ADD_SUBDIRECTORY(Applications/DTIAtlasFiberAnalyzer/Testing)
    ADD_SUBDIRECTORY(Applications/dtitractstat/Testing)
    ADD_SUBDIRECTORY(Applications/FiberCompare/Testing)
    ADD_SUBDIRECTORY(Applications/FiberPostProcess/Testing)
    ADD_SUBDIRECTORY(Applications/MergeStatWithFiber/Testing)
  ENDIF(BUILD_TESTING)
endif()
