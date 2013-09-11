cmake_minimum_required(VERSION 2.8.7)

set(LOCAL_PROJECT_NAME DTIAtlasFiberAnalyzer)

set( ${LOCAL_PROJECT_NAME}_USE_QT ON )
# Platform check
#-----------------------------------------------------------------------------

set(PLATFORM_CHECK true)

if(PLATFORM_CHECK)
  # See CMake/Modules/Platform/Darwin.cmake)
  #   6.x == Mac OSX 10.2 (Jaguar)
  #   7.x == Mac OSX 10.3 (Panther)
  #   8.x == Mac OSX 10.4 (Tiger)
  #   9.x == Mac OSX 10.5 (Leopard)
  #  10.x == Mac OSX 10.6 (Snow Leopard)
  if (DARWIN_MAJOR_VERSION LESS "9")
    message(FATAL_ERROR "Only Mac OSX >= 10.5 are supported !")
  endif()
endif()

#-----------------------------------------------------------------------------
# Extension option(s)
#-----------------------------------------------------------------------------
unsetForSlicer( NAMES CMAKE_MODULE_PATH CMAKE_C_COMPILER CMAKE_CXX_COMPILER ITK_DIR SlicerExecutionModel_DIR VTK_DIR QT_QMAKE_EXECUTABLE)
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})
unsetAllForSlicerBut( NAMES ITK_DIR SlicerExecutionModel_DIR VTK_DIR QT_QMAKE_EXECUTABLE )
resetForSlicer( NAMES CMAKE_MODULE_PATH CMAKE_C_COMPILER CMAKE_CXX_COMPILER )
if( APPLE )
  set( CMAKE_EXE_LINKER_FLAGS -Wl,-rpath,@loader_path/../../../../../)
endif()

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


#------------------------------------------------------------------------------
# ${LOCAL_PROJECT_NAME} dependency list
#------------------------------------------------------------------------------

set(${LOCAL_PROJECT_NAME}_DEPENDENCIES QWT)

if(BUILD_STYLE_UTILS)
  list(APPEND ${LOCAL_PROJECT_NAME}_DEPENDENCIES Cppcheck KWStyle Uncrustify)
endif()


#-----------------------------------------------------------------------------
# Define Superbuild global variables
#-----------------------------------------------------------------------------

# This variable will contain the list of CMake variable specific to each external project
# that should passed to ${CMAKE_PROJECT_NAME}.
# The item of this list should have the following form: <EP_VAR>:<TYPE>
# where '<EP_VAR>' is an external project variable and TYPE is either BOOL, STRING, PATH or FILEPATH.
# TODO Variable appended to this list will be automatically exported in ${LOCAL_PROJECT_NAME}Config.cmake,
# prefix '${LOCAL_PROJECT_NAME}_' will be prepended if it applies.
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
  CMAKE_SHARED_LINKER_FLAGS:STRING
  CMAKE_EXE_LINKER_FLAGS:STRING
  CMAKE_MODULE_LINKER_FLAGS:STRING
  SITE:STRING
  BUILDNAME:STRING
  Subversion_SVN_EXECUTABLE:FILEPATH
  )

if(${LOCAL_PROJECT_NAME}_USE_QT)
  list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS
    ${LOCAL_PROJECT_NAME}_USE_QT:BOOL
    QT_QMAKE_EXECUTABLE:PATH
    QT_MOC_EXECUTABLE:PATH
    QT_UIC_EXECUTABLE:PATH
    )
endif()

_expand_external_project_vars()
set(COMMON_EXTERNAL_PROJECT_ARGS ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS})
SlicerMacroCheckExternalProjectDependency(${LOCAL_PROJECT_NAME})

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
  ITK_VERSION_MAJOR:STRING
  ITK_DIR:PATH
  VTK_DIR:PATH
  GenerateCLP_DIR:PATH
  SlicerExecutionModel_DIR:PATH
  CMAKE_MODULE_PATH:PATH
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


set(proj dti_tract_stat)
ExternalProject_Add(${proj}
  DOWNLOAD_COMMAND ""
  INSTALL_COMMAND ""
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
  BINARY_DIR ${proj}-build
  DEPENDS ${${LOCAL_PROJECT_NAME}_DEPENDENCIES}
  CMAKE_GENERATOR ${gen}
  CMAKE_ARGS
    -DQWT_LIBRARY_PATH:FILEPATH=${QWT_LIBRARY_PATH}
    -DQWT_INCLUDE_DIR:PATH=${QWT_INCLUDE_DIR}
    -DCOMPILE_EXTERNAL_DTIPROCESS:BOOL=ON
    -DCOMPILE_MERGERSTATWITHFIBER:BOOL=ON
    -DCOMPILE_FIBERCOMPARE:BOOL=ON
    -DCOMPILE_DTITRACTSTAT:BOOL=ON
    -DCOMPILE_DTIATLASFIBERANALYZER:BOOL=ON
    -DMIDAS_PACKAGE_EMAIL:STRING=${MIDAS_PACKAGE_EMAIL}
    -DMIDAS_PACKAGE_API_KEY:STRING=${MIDAS_PACKAGE_API_KEY}
    -DEXTENSION_NAME:STRING=${EXTENSION_NAME}
    -DEXTENSION_SUPERBUILD_BINARY_DIR:PATH=${${EXTENSION_NAME}_BINARY_DIR}
    -DDTIAtlasFiberAnalyzer_BUILD_SLICER_EXTENSION:BOOL=OFF
     ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
     ${COMMON_EXTERNAL_PROJECT_ARGS}
    # Slicer
    -DSlicer_DIR:PATH=${Slicer_DIR}
    -DSlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY:PATH=${SlicerExecutionModel_DEFAULT_CLI_RUNTIME_OUTPUT_DIRECTORY}
    -DSlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION:PATH=${SlicerExecutionModel_DEFAULT_CLI_INSTALL_RUNTIME_DESTINATION}
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

