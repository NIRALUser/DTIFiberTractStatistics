if( NOT EXTERNAL_SOURCE_DIRECTORY )
  set( EXTERNAL_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ExternalSources )
endif()

# Make sure this file is included only once by creating globally unique varibles
# based on the name of this included file.
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

## External_${extProjName}.cmake files can be recurisvely included,
## and cmake variables are global, so when including sub projects it
## is important make the extProjName and proj variables
## appear to stay constant in one of these files.
## Store global variables before overwriting (then restore at end of this file.)
ProjectDependancyPush(CACHED_extProjName ${extProjName})
ProjectDependancyPush(CACHED_proj ${proj})

# Make sure that the ExtProjName/IntProjName variables are unique globally
# even if other External_${ExtProjName}.cmake files are sourced by
# SlicerMacroCheckExternalProjectDependency
set(extProjName VTK) #The find_package known name
set(proj        VTK) #This local name

set(${extProjName}_REQUIRED_VERSION "5.10")  #If a required version is necessary, then set this, else leave blank

# Sanity checks
#if(DEFINED ${extProjName}_DIR AND NOT EXISTS ${${extProjName}_DIR})
#  message(FATAL_ERROR "${extProjName}_DIR variable is defined but corresponds to non-existing directory (${${extProjName}_DIR})")
#endif()

# Set dependency list

if(USE_QT4)
  set(${proj}_DEPENDENCIES Qt4)
  set(VTK_QT_VERSION 4)
else()
  set(VTK_QT_VERSION 5)
  set(${proj}_DEPENDENCIES Qt5)
endif()

# Include dependent projects if any
SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT ( DEFINED "USE_SYSTEM_${extProjName}" AND "${USE_SYSTEM_${extProjName}}" ) )
  #message(STATUS "${__indent}Adding project ${proj}")

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
      )
  endif()

  set(${proj}_CMAKE_OPTIONS
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/${proj}-install
      -DCMAKE_INSTALL_LIBDIR:PATH=${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/lib
      -DBUILD_EXAMPLES:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF      
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      -DVTK_Group_Qt:BOOL=ON 
      -DQT_QMAKE_EXECUTABLE:PATH=${QT_QMAKE_EXECUTABLE}
      -DCMAKE_PREFIX_PATH=${Qt5_DIR}
      -DVTK_QT_VERSION:STRING=${VTK_QT_VERSION}
    )
  ### --- End Project specific additions

  
  set(${proj}_REPOSITORY ${git_protocol}://github.com/Kitware/VTK.git)
  set(${proj}_GIT_TAG "v8.2.0")
  # set(${proj}_GIT_TAG "v5.10.0")
  
  ExternalProject_Add(${proj}
    GIT_REPOSITORY ${${proj}_REPOSITORY}
    GIT_TAG ${${proj}_GIT_TAG}
    SOURCE_DIR ${EXTERNAL_SOURCE_DIRECTORY}/${proj}
    BINARY_DIR ${proj}-build
    BUILD_COMMAND ${VTK_BUILD_STEP}
    LOG_CONFIGURE 0  # Wrap configure in script to ignore log output from dashboards
    LOG_BUILD     0  # Wrap build in script to to ignore log output from dashboards
    LOG_TEST      0  # Wrap test in script to to ignore log output from dashboards
    LOG_INSTALL   0  # Wrap install in script to to ignore log output from dashboards
    ${cmakeversion_external_update} "${cmakeversion_external_update_value}"
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${COMMON_EXTERNAL_PROJECT_ARGS}
      ${${proj}_CMAKE_OPTIONS}
## We really do want to install in order to limit # of include paths INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

  set(LIB_DIR lib)
  # if(EXISTS ${CMAKE_BINARY_DIR}/${proj}-install/${LIB_DIR})
  #   set(LIB_DIR lib)
  # else()
  #   set(LIB_DIR lib64)
  # endif()
  set(${extProjName}_DIR ${EXTERNAL_BINARY_DIRECTORY}/${proj}-install/lib/cmake/vtk-8.2)
  set(${extProjName}_INCLUDE_DIR ${EXTERNAL_BINARY_DIRECTORY}/${proj}-install/include/vtk-8.2)
  # set(${extProjName}_DIR ${EXTERNAL_BINARY_DIRECTORY}/${proj}-install/lib/vtk-5.10)
  #set(${extProjName}_DIR ${CMAKE_BINARY_DIR}/${proj}-build)

else()
  if(${USE_SYSTEM_${extProjName}})
    find_package(${extProjName} ${${extProjName}_REQUIRED_VERSION} REQUIRED)
    message("USING the system ${extProjName}, set ${extProjName}_DIR=${${extProjName}_DIR}")
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}, let's add an 'empty' one
  SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)
list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_INCLUDE_DIR:PATH)
_expand_external_project_vars()
set(COMMON_EXTERNAL_PROJECT_ARGS ${${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_ARGS})

ProjectDependancyPop(CACHED_extProjName extProjName)
ProjectDependancyPop(CACHED_proj proj)
