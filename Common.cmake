include(CMakeDependentOption)

enable_language(C)
enable_language(CXX)



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

option( BUILD_TESTING   "Build the testing tree" ON )


option(USE_GIT_PROTOCOL "If behind a firewall turn this off to use http instead." ON)
set(git_protocol "git")
if(NOT USE_GIT_PROTOCOL)
  set(git_protocol "http")
else(NOT USE_GIT_PROTOCOL)
  set(git_protocol "git")
endif()


# With CMake 2.8.9 or later, the UPDATE_COMMAND is required for updates to occur.
# For earlier versions, we nullify the update state to prevent updates and
# undesirable rebuild.
if(CMAKE_VERSION VERSION_LESS 2.8.9)
  set(cmakeversion_external_update UPDATE_COMMAND "")
else()
  set(cmakeversion_external_update LOG_UPDATE 1)
endif()
 
#-----------------------------------------------------------------------------
# Update CMake module path
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_CURRENT_BINARY_DIR}/CMake
  ${CMAKE_MODULE_PATH}
  )

#-----------------------------------------------------------------------------
# Sanity checks
#------------------------------------------------------------------------------
include(PreventInSourceBuilds)
include(PreventInBuildInstalls)
include( setSlicerConfigVarList )
#-----------------------------------------------------------------------------
# CMake Function(s) and Macro(s)
#-----------------------------------------------------------------------------
if(CMAKE_PATCH_VERSION LESS 3)
  include(Pre283CMakeParseArguments)
else()
  include(CMakeParseArguments)
endif()


macro( unsetForSlicer )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Unsetting "${var} - old value: ${${var}} )
    endif()
    set( ${var}_TMP ${${var}} )
    unset( ${var} CACHE )
    unset( ${var} )
  endforeach()
endmacro()

macro( resetForSlicer )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    if( "${${var}_TMP}" STREQUAL "" )
      message( "Does not reset ${var}. ${var}_TMP was empty" )
    else()
      set( ${var} ${${var}_TMP} CACHE PATH "${var} PATH" FORCE )
      unset( ${var}_TMP )
      if( TMP_VERBOSE )
        message( "resetting ${var} - new value: "${${var}} )
      endif()
    endif()
  endforeach()
endmacro()

macro( unsetAllForSlicerBut )
  set(options VERBOSE )
  set(oneValueArgs )
  set(multiValueArgs NAMES )
  CMAKE_PARSE_ARGUMENTS(TMP
    "${options}"
    "${oneValueArgs}"
    "${multiValueArgs}"
    ${ARGN}
    )
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Saving ${var} - value: ${${var}}" )
    endif()
    set( ${var}_TMP ${${var}} )
  endforeach()
  foreach( var ${SlicerConfigVarList} )
    unset( ${var} CACHE )
    unset( ${var} )
  endforeach()
  foreach( var ${TMP_NAMES} )
    if( TMP_VERBOSE )
      message( "Writing back ${var} - value: ${${var}_TMP}")
    endif()
    set( ${var} ${${var}_TMP} )
  endforeach()
endmacro()


include(SlicerMacroEmptyExternalProject)
include(SlicerMacroCheckExternalProjectDependency)
include(ExternalProject)
if(CMAKE_EXTRA_GENERATOR)
  set(gen "${CMAKE_EXTRA_GENERATOR} - ${CMAKE_GENERATOR}")
else()
  set(gen "${CMAKE_GENERATOR}")
endif()

#-------------------------------------------------------------------------
# Augment compiler flags
#-------------------------------------------------------------------------
include(ITKSetStandardCompilerFlags)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_DEBUG_DESIRED_FLAGS}" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_DEBUG_DESIRED_FLAGS}" )
else() # Release, or anything else
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_RELEASE_DESIRED_FLAGS}" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_RELEASE_DESIRED_FLAGS}" )
endif()

#-----------------------------------------------------------------------------
# Add needed flag for gnu on linux like enviroments to build static common libs
# suitable for linking with shared object libs.
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
  if(NOT "${CMAKE_CXX_FLAGS}" MATCHES "-fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  endif()
  if(NOT "${CMAKE_C_FLAGS}" MATCHES "-fPIC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  endif()
endif()
